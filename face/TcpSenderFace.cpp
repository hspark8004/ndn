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
#include "Container.hpp"
#include "face/TcpSenderFace.hpp"
#include "tlv_type.hpp"
#include "tlv_length.hpp"
#include <errno.h>

using namespace std;

TcpSenderFace::TcpSenderFace(Container* container, string name, int port)
  : p_container(container), m_name(name), m_port(port)
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

  cout << "Accept: " << clntfd << endl;

  struct event* e = event_new(eventBase, clntfd, EV_READ | EV_PERSIST,
    TcpSenderFace::onReadSocket, arg);

  if(event_add(e, NULL) < 0) {
    throw runtimeError(&errno);
  }

  (*pThis->getSocketEventMap())[clntfd] = e;

  pThis->getContainer()->getClientConnectionMap()->insert({clntfd, pThis});

  // TODO send Interest for connection.
  string _name(pThis->getName());
  _name = _name.append("/").append(to_string(clntfd));

  tlv_type tlvType(1);
  tlv_length tlvLength(_name.size(), 0);

  char* name = new char[sizeof(tlv_type) + sizeof(tlv_length) + _name.size() + 1];
  memcpy(name, &tlvType, sizeof(tlv_type));
  memcpy(name + sizeof(tlv_type), &tlvLength, sizeof(tlv_length));
  copy(_name.begin(), _name.end(), name + sizeof(tlv_type) + sizeof(tlv_length));
  _name[sizeof(tlv_type) + sizeof(tlv_length) + _name.size()] = '\0';

  pThis->sendInterest(clntfd, name, sizeof(tlv_type) + sizeof(tlv_length) + _name.size());
}

void
TcpSenderFace::onReadSocket(evutil_socket_t fd, short events, void* arg)
{
  cout << "Read File: " << fd << endl;

  TcpSenderFace* pThis = (TcpSenderFace*)arg;
  char buf[BUFSIZ];
  ssize_t len = recv(fd, buf, BUFSIZ, 0);
  string data(buf, len); // data after name.
  string prefix(pThis->getName());

  sprintf(buf, "%d", fd); // Get file descriptor.
  prefix = prefix.append("/").append(buf); // Append file descriptor.
  tlv_type tlvType(1);

  if(len == 0) { // Gracefully closed.
    auto socketEventMap = pThis->getSocketEventMap();

    // Send close packet.
    data = prefix.append("/");
    tlv_length tlvLength(data.size(), 0);

    char* name = new char[sizeof(tlv_type) + sizeof(tlv_length) + data.size() + 1];
    memcpy(name, &tlvType, sizeof(tlv_type));
    memcpy(name + sizeof(tlv_type), &tlvLength, sizeof(tlv_length));
    copy(data.begin(), data.end(), name + sizeof(tlv_type) + sizeof(tlv_length));
    name[sizeof(tlv_type) + sizeof(tlv_length) + data.size()] = '\0';

    cout << "Name Length: " << tlvLength.getNameLength() << endl;
    cout << "Close pakcet: " << name + sizeof(tlv_type) + sizeof(tlv_length) << endl;
    pThis->sendInterest(fd, name, sizeof(tlv_type) + sizeof(tlv_length) + data.size());

    event_free((*socketEventMap)[fd]);
    close(fd);
    socketEventMap->erase(fd);

    return;
  }

  data = prefix.append("/").append(urlEncode(data));
  tlv_length tlvLength(data.size(), 0);

  char* name = new char[sizeof(tlv_type) + sizeof(tlv_length) + data.size() + 1];
  memcpy(name, &tlvType, sizeof(tlv_type));
  memcpy(name + sizeof(tlv_type), &tlvLength, sizeof(tlv_length));
  copy(data.begin(), data.end(), name + sizeof(tlv_type) + sizeof(tlv_length));
  name[sizeof(tlv_type) + sizeof(tlv_length) + data.size()] = '\0';

  cout << "Name Length: " << tlvLength.getNameLength() << endl;
  cout << "Data: " << name + sizeof(tlv_type) + sizeof(tlv_length) << endl;
  pThis->sendInterest(fd, name, sizeof(tlv_type) + sizeof(tlv_length) + data.size());

/*
  cout << "Data: " << data << endl;
  cout << "Prefix: " << prefix << endl;
  cout <<  << endl;
  Interest interest(prefix.append("/").append(urlEncode(data))); // Append data.
  cout << "Send pakcet: " << interest.getName() << endl;
  // pThis->sendInterest(interest.getName().c_str(), interest.getName().length());
  pThis->sendInterest(fd, interest.getName(), strlen(interest.getName()) + len);
*/
}

void
TcpSenderFace::onReceiveData(char* name, unsigned char* data, size_t size)
{
    cout << "TcpSenderFace::onReceiveData" << endl;

    string str(name);
    string PrefixName = getPrefix(str); // name
    string fd = getData(str);           // client fd

    int clntfd = stoi(fd);              // casting
    //sprintf(clntfd, "%s", fd);

    send(clntfd, data, size, 0);
    
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
