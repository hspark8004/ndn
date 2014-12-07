#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <utility>
#include <unordered_map>
#include <sstream>
#include "Common.hpp"
#include "Container.hpp"
#include "face/TcpSenderFace.hpp"
#include "tlv_type.hpp"
#include "tlv_length.hpp"

using namespace std;

static bool
dataSort(const segment_t& a, const segment_t& b)
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

  ostringstream stream;
  stream << pThis->getName() << "/" << to_string(clntfd) << "/";

  Interest interest(stream.str());
  pThis->sendInterest(interest);
}

void
TcpSenderFace::onReadSocket(evutil_socket_t fd, short events, void* arg)
{
  TcpSenderFace* pThis = (TcpSenderFace*)arg;
  char buf[BUFSIZ];
  ssize_t len = recv(fd, buf, BUFSIZ, 0);
  ostringstream stream;
  stream << pThis->getName() << "/" << to_string(fd) << "/";

  if(len > 0) {
    stream << urlEncode(string(buf, len));
  }

  Interest interest(stream.str());
  pThis->sendInterest(interest);

  if(len == 0) { // Socket closed.
    event_free((*pThis->getSocketEventMap())[fd]);
    close(fd);
    pThis->getSocketEventMap()->erase(fd);
  }
}

void
TcpSenderFace::onReceiveData(Data& data)
{
  string prefix(data.extractComName());
  int fd = data.extractAppName();
  int segment = data.extractDataNum();
  auto buf = data.getContent();
  auto len = data.getContentSize();

  if(segment == m_segmentIndex) {
    send(fd, buf, len, 0);
    m_segmentIndex++;

    for(vector<segment_t>::iterator iter = m_segmentList.begin(); iter != m_segmentList.end(); iter++) {
      if(m_segmentIndex == get<0>(*iter)) {
        send(fd, get<1>(*iter), get<2>(*iter), 0);
        m_segmentList.erase(iter);
        m_segmentIndex++;
        delete get<1>(*iter);
      }
    }
  } else if(segment > m_segmentIndex) {
    m_segmentList.push_back(make_tuple(segment, buf, len));
    sort(m_segmentList.begin(), m_segmentList.end(), dataSort);
  }
}

void
TcpSenderFace::sendInterest(Interest& interest)
{
  p_container->getNdnLayer()->sendInterest(interest);
}

inline vector<tuple<uint64_t, unsigned char*, size_t>>
TcpSenderFace::getSegmentList()
{
  return m_segmentList;
}

inline uint64_t
TcpSenderFace::getSegmentIndex()
{
  return m_segmentIndex;
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
