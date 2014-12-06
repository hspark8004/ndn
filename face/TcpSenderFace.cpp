#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <tuple>
#include <vector>
#include <utility>
#include <unordered_map>
#include "Common.hpp"
#include "Container.hpp"
#include "face/TcpSenderFace.hpp"
#include "tlv_type.hpp"
#include "tlv_length.hpp"
#include <errno.h>
#include <cctype>

using namespace std;

typedef tuple<uint64_t, unsigned char*, size_t> mytuple;

static char*
makeInterest(string name, int* len)
{
  tlv_type type(1);
  tlv_length length(name.size(), 0);

  char* packet = new char[sizeof(type) + sizeof(length) + name.size() + 1];
  memcpy(packet, &type, sizeof(type));
  memcpy(packet + sizeof(type), &length, sizeof(length));
  copy(name.begin(), name.end(), packet + sizeof(type) + sizeof(length));
  packet[sizeof(type) + sizeof(length) + name.size()] = 0;

  *len = sizeof(type) + sizeof(length) + name.size();

  return packet;
}

static bool
dataSort(const mytuple& a, const mytuple& b)
{
  return get<0>(a) < get<0>(b);
}

TcpSenderFace::TcpSenderFace(Container* container, string name, int port)
  : p_container(container), m_name(name), m_port(port)
{
  struct sockaddr_in servaddr;

  p_socketEventMap = new unordered_map<int, struct event*>;

  if((m_servfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    throw runtimeError(&errno);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htons(INADDR_ANY);
  servaddr.sin_port = htons(m_port);

  if(bind(m_servfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
    throw runtimeError(&errno);
  }

  if(listen(m_servfd, m_backlog) < 0) {
    throw runtimeError(&errno);
  }

  p_event = event_new(eventBase, m_servfd, EV_READ | EV_PERSIST,
    TcpSenderFace::onAcceptSocket, (void*)this);

  if(event_add(p_event, NULL) < 0) {
    throw runtimeError(&errno);
  }
}

TcpSenderFace::~TcpSenderFace()
{
  auto iter = p_socketEventMap->begin();

  while(iter != p_socketEventMap->end()) {
    event_free(iter->second);
    close(iter->first);
    p_socketEventMap->erase(iter);

    iter++;
  }

  event_free(p_event);
  close(m_servfd);
}

void
TcpSenderFace::onAcceptSocket(evutil_socket_t fd, short events, void* arg)
{
  TcpSenderFace* pThis = (TcpSenderFace*)arg;
  struct sockaddr_in clntaddr;
  socklen_t addrlen = sizeof(clntaddr);
  int clntfd;
  int len;

  if((clntfd = accept(fd, (struct sockaddr*)&clntaddr, &addrlen)) < 0) {
    throw runtimeError(&errno);
  }

  struct event* e = event_new(eventBase, clntfd, EV_READ | EV_PERSIST,
    TcpSenderFace::onReadSocket, arg);

  if(event_add(e, NULL) < 0) {
    throw runtimeError(&errno);
  }

  (*pThis->getSocketEventMap())[clntfd] = e;

  pThis->getContainer()->getClientConnectionMap()->insert({clntfd, pThis});

  char* interest = makeInterest(pThis->getName().append("/").append(to_string(clntfd)).append("/"), &len);
  cout << "Interest: " << pThis->getName().append("/").append(to_string(clntfd)).append("/") << endl;
  pThis->sendInterest(clntfd, interest, len);
}

void
TcpSenderFace::onReadSocket(evutil_socket_t fd, short events, void* arg)
{
  TcpSenderFace* pThis = (TcpSenderFace*)arg;
  char buf[BUFSIZ];
  char *packet;
  ssize_t ret;
  int len;

  if((ret = recv(fd, buf, BUFSIZ, 0)) == 0) { // Gracefully closed.
    auto socketEventMap = pThis->getSocketEventMap();

    // Send close packet.
    packet = makeInterest(pThis->getName().append("/").append(to_string(fd)).append("/"), &len);
    cout << "Interest: " << pThis->getName().append("/").append(to_string(fd)).append("/") << endl;
    pThis->sendInterest(fd, packet, len);

    event_free((*socketEventMap)[fd]);
    close(fd);
    socketEventMap->erase(fd);

    return;
  }

  // Send interest packet.
  packet = makeInterest(pThis->getName().append("/").append(to_string(fd)).append("/").append(buf, ret), &len);
  cout << "Interest: " << pThis->getName().append("/").append(to_string(fd)).append("/") << endl;
  pThis->sendInterest(fd, packet, len);
}

void
TcpSenderFace::onReceiveData(char* _name, unsigned char* buf, size_t len)
{
  string name(_name);
  string prefix = getPrefix(getPrefix(name));
  int clntfd = stoi(getPrefix(prefix));
  int index = stoi(getData(name));

  // TODO process index
  if(m_dataIndex == index) {
    send(clntfd, buf, len, 0);
    m_dataIndex++;

    delete buf;

    // TODO process list
    for(vector<mytuple>::iterator iter = m_dataList.begin(); iter != m_dataList.end(); iter++) {
      if(m_dataIndex == get<0>(*iter)) {
        send(clntfd, get<1>(*iter), get<2>(*iter), 0);
        m_dataIndex++;

        delete get<1>(*iter);
      }
    }
  } else if(index > m_dataIndex) {
    m_dataList.push_back(make_tuple(index, buf, len));

    sort(m_dataList.begin(), m_dataList.end(), dataSort);
  }
}

void
TcpSenderFace::sendInterest(int fd, char* name, uint64_t len)
{
  size_t ret;

  p_container->getLinkLayer()->sendInterest(fd, (unsigned char*)name, len);
  delete name;

#ifdef __DEBUG_MODE
  if((ret = send(m_sendSocket, name, len, 0)) < 0) {
    perror("send");
  }
#endif /* __DEBUG_MODE */
}

inline vector<tuple<uint64_t, unsigned char*, size_t>>
TcpSenderFace::getDataList()
{
  return m_dataList;
}

inline uint64_t
TcpSenderFace::getDataIndex()
{
  return m_dataIndex;
}

inline unordered_map<int, struct event*>*
TcpSenderFace::getSocketEventMap()
{
  return p_socketEventMap;
}

inline string
TcpSenderFace::getName()
{
  return m_name;
}

inline int
TcpSenderFace::getType()
{
  return FACE_TCP_SENDER;
}
#ifdef __DEBUG_MODE
void
TcpSenderFace::setSendSocket(int fd)
{
  m_sendSocket = fd;
}

void
TcpSenderFace::setRecvSocket(int fd)
{
  m_recvSocket = fd;
}
#endif /* __DEBUG_MODE */
