#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <iterator>
#include <utility>
#include <unordered_map>
#include "Common.hpp"
#include "Interest.hpp"
#include "face/TcpReceiverFace.hpp"

using namespace std;

TcpReceiverFace::TcpReceiverFace(string name, int port)
  : m_name(name), m_port(port)
{
  p_socketEventMap = new unordered_map<int, struct event*>;
}

TcpReceiverFace::~TcpReceiverFace()
{
  for(auto iter = p_socketEventMap->begin(); iter != p_socketEventMap->end(); iter++) {
    event_free(iter->second);
    close(iter->first);

    p_socketEventMap->erase(iter);
  }
}

int
TcpReceiverFace::createConnection()
{
  struct event* e;
  struct sockaddr_in servaddr;
  socklen_t addrlen = sizeof(servaddr);
  int clntfd;
  int servfd;

  if((clntfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    throw runtimeError(&errno);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(m_port);

  if((servfd = connect(clntfd, (struct sockaddr*)&servaddr, addrlen)) < 0) {
//    throw runtimeError(&errno);
    return -1; // Fail to connect legacy server.
  }

  e = event_new(eventBase, servfd, EV_READ | EV_PERSIST,
    TcpReceiverFace::onReadSocket, (void*)this);

  (*p_socketEventMap)[servfd] = e;

  return clntfd;
}

void
TcpReceiverFace::onReadSocket(evutil_socket_t fd, short events, void* arg)
{
  TcpReceiverFace* pThis = (TcpReceiverFace*)arg;
  char buf[BUFSIZ];
  ssize_t len = recv(fd, buf, BUFSIZ, 0);
  string data(buf, len);
  string prefix(pThis->getName());

  sprintf(buf, "%d", fd); // Get file descriptor.
  prefix.append("/").append(buf); // Append file descriptor.

  cout << "READ!!!!!" << endl;

  if(len == 0) {
    auto socketEventMap = pThis->getSocketEventMap();

    event_free((*socketEventMap)[fd]);
    close(fd);
    socketEventMap->erase(fd);

    return;
  }
}

void
TcpReceiverFace::onReceiveInterest(char* interest)
{
  string name = getPrefix(interest);
  string prefix = getPrefix(name);
  string data = urlDecode(getData(interest));
  int clntfd = atoi(getData(name).c_str());
  int servfd = m_connectionMap[clntfd];

  if(data.length() == 0) {
    if(servfd > 0) {
      m_connectionMap.erase(clntfd);
      close(servfd);
    }
  } else {
    if(servfd == 0) {
      servfd = createConnection();
      m_connectionMap[clntfd] = servfd;
    }

    send(servfd, data.c_str(), data.length(), 0);
  }
}

inline unordered_map<int, struct event*>*
TcpReceiverFace::getSocketEventMap()
{
  return p_socketEventMap;
}

#ifdef __DEBUG_MODE
void
TcpReceiverFace::onTest(evutil_socket_t fd, short events, void* arg)
{
  TcpReceiverFace* pThis = (TcpReceiverFace*)arg;
  char buf[BUFSIZ];
  memset(buf, 0, BUFSIZ);
  ssize_t len = recv(fd, buf, BUFSIZ, 0);

  if(len == 0) { // Gracefully closed.
    auto socketEventMap = pThis->getSocketEventMap();

    event_free((*socketEventMap)[fd]);
    close(fd);
    socketEventMap->erase(fd);

    return;
  }

  pThis->onReceiveInterest(buf);
}

void
TcpReceiverFace::setSendSocket(int fd)
{
  m_sendSocket = fd;
}

void
TcpReceiverFace::setRecvSocket(int fd)
{
  m_recvSocket = fd;

  struct event* e = event_new(eventBase, m_recvSocket,
    EV_READ | EV_PERSIST, onTest, (void*)this);

  if(event_add(e, NULL) < 0) {
    throw runtimeError(&errno);
  }
}

int
TcpReceiverFace::getRecvSocket()
{
  return m_recvSocket;
}

inline string
TcpReceiverFace::getName()
{
  return m_name;
}
#endif /* __DEBUG_MODE */
