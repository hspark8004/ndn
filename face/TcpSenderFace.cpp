#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <iostream>
#include <iterator>
#include <utility>
#include <unordered_map>
#include "Common.hpp"
#include "face/TcpSenderFace.hpp"
#include <errno.h>

using namespace std;

TcpSenderFace::TcpSenderFace(string name, int port)
  : m_name(name), m_port(port)
{
  p_socketEventMap = new unordered_map<int, struct event*>;

  initSocket();
  initEvent();
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

inline void
TcpSenderFace::initSocket()
{
  if((m_servfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    throw runtimeError(&errno);
  }

  memset(&m_servaddr, 0, sizeof(m_servaddr));
  m_servaddr.sin_family = AF_INET;
  m_servaddr.sin_addr.s_addr = htons(INADDR_ANY);
  m_servaddr.sin_port = htons(m_port);

  m_addrlen = sizeof(m_servaddr);

  if(bind(m_servfd, (struct sockaddr*)&m_servaddr, m_addrlen) < 0) {
    throw runtimeError(&errno);
  }

  if(listen(m_servfd, m_backlog) < 0) {
    throw runtimeError(&errno);
  }
}

inline void
TcpSenderFace::initEvent()
{
  p_event = event_new(eventBase, m_servfd, EV_READ | EV_PERSIST,
    TcpSenderFace::onAcceptSocket, (void*)this);

  if(event_add(p_event, NULL) < 0) {
    throw runtimeError(&errno);
  }
}

void
TcpSenderFace::onAcceptSocket(evutil_socket_t fd, short events, void* arg)
{
  TcpSenderFace* pThis = (TcpSenderFace*)arg;
  struct sockaddr_in clntaddr;
  socklen_t addrlen = sizeof(clntaddr);
  int clntfd;

  if((clntfd = accept(fd, (struct sockaddr*)&clntaddr, &addrlen)) < 0) {
    throw runtimeError(&errno);
  }

  struct event* e = event_new(eventBase, clntfd, EV_READ | EV_PERSIST,
    TcpSenderFace::onReadSocket, arg);

  if(event_add(e, NULL) < 0) {
    throw runtimeError(&errno);
  }

  (*pThis->getSocketEventMap())[clntfd] = e;
}

void
TcpSenderFace::onReadSocket(evutil_socket_t fd, short events, void* arg)
{
  TcpSenderFace* pThis = (TcpSenderFace*)arg;
  char buf[BUFSIZ];
  ssize_t len = recv(fd, buf, BUFSIZ, 0);
  string data(buf, len); // data after name.
  string prefix(pThis->getName());

  sprintf(buf, "%d", fd); // Get file descriptor.
  prefix.append("/").append(buf); // Append file descriptor.

  if(len == 0) { // Gracefully closed.
    auto socketEventMap = pThis->getSocketEventMap();

    // Send close packet.
    Interest interest(prefix.append("/")); // Data length is zero.
    // pThis->sendInterest(interest.getName().c_str(), interest.getName().length());
    pThis->sendInterest(interest.getName(), strlen(interest.getName()));

    cout << "Close pakcet: " << interest.getName() << endl;

    event_free((*socketEventMap)[fd]);
    close(fd);
    socketEventMap->erase(fd);

    return;
  }

  Interest interest(prefix.append("/").append(urlEncode(data))); // Append data.
  cout << "Send pakcet: " << interest.getName() << endl;
  // pThis->sendInterest(interest.getName().c_str(), interest.getName().length());
  pThis->sendInterest(interest.getName(), strlen(interest.getName()) + len);
}

void
TcpSenderFace::onReceiveData(char* name, unsigned char* data, size_t size)
{
}

void
TcpSenderFace::sendInterest(const char* name, size_t len)
{
  size_t ret;

#ifdef __DEBUG_MODE
  if((ret = send(m_sendSocket, name, len, 0)) < 0) {
    perror("send");
  }
#endif /* __DEBUG_MODE */
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
