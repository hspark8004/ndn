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
#include <sstream>
#include <unordered_map>
#include "Common.hpp"
#include "Interest.hpp"
#include "face/TcpReceiverFace.hpp"
#include "Container.hpp"

using namespace std;

TcpReceiverFace::TcpReceiverFace(Container* container, string name, int port)
  : p_container(container), m_name(name), m_port(port)
{
  p_socketEventMap = new unordered_map<int, struct event*>;
  p_connectionMap = new unordered_map<int, int>;
  p_dataIndex = new uint64_t(1);

  (*p_container->getServerConnectionMap())[serverFaceId++] = this;
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
TcpReceiverFace::createSocketEvent()
{
  struct sockaddr_in servaddr;
  struct event* e;
  int clntfd;
  int ret;

  if((clntfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    throw runtimeError(&errno);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(m_port);

  if((ret = connect(clntfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) < 0) {
    throw runtimeError(&errno);
  }

  e = event_new(eventBase, clntfd, EV_READ | EV_PERSIST,
    TcpReceiverFace::onReadSocket, (void*)this);

  if(event_add(e, NULL) < 0) {
    throw runtimeError(&errno);
  }

  (*p_socketEventMap)[clntfd] = e;

  return clntfd;
}

void
TcpReceiverFace::removeSocketEvent(int fd)
{
  struct event* e = (*p_socketEventMap)[fd];

  close(fd);
  event_free(e);
  p_socketEventMap->erase(fd);
}

void
TcpReceiverFace::onReadSocket(evutil_socket_t fd, short events, void* arg)
{
  TcpReceiverFace* pThis = (TcpReceiverFace*)arg;
  unordered_map<int, int>* p_connectionMap = pThis->getConnectionMap();
  char buf[BUFSIZ];
  ssize_t len = recv(fd, buf, BUFSIZ, 0);

  if(len == 0) {
    pThis->removeSocketEvent(fd);
    return;
  }

  for(auto iter = p_connectionMap->begin(); iter != p_connectionMap->end(); iter++) {
    if(fd == iter->second) {
      int clntfd = iter->first;
      uint64_t segment = *pThis->getSegmentIndex();
      pThis->setSegmentIndex(segment + 1);

      ostringstream stream;
      stream << pThis->getName() << "/" << to_string(clntfd) << "/" << to_string(segment);
      string content = string(buf, len);

      Data data(const_cast<char*>(stream.str().c_str()),
        reinterpret_cast<unsigned char*>(const_cast<char*>(content.c_str())),
        content.size());
      pThis->sendData(data);
      return;
    }
  }
}

void
TcpReceiverFace::sendData(Data& data)
{
  p_container->getNdnLayer()->sendData(data);
}

void
TcpReceiverFace::onReceiveInterest(Interest& interest, uint8_t* macAddress)
{
  int clntfd = interest.extractFileDescriptor();
  int servfd = (*p_connectionMap)[clntfd];
  string data(interest.extractReqName());

  if((data.length() == 0) && (servfd > 0)) { // Receive close message and socket exists.
    removeSocketEvent(servfd);

    for(auto iter = rib.begin(); iter != rib.end(); iter++) {
      if((memcmp(macAddress, iter->getMacAddress(), sizeof(uint8_t) * 6) == 0)
        && (clntfd == iter->getClientFd())) {
        rib.erase(iter);
        break;
      }
    }

    p_connectionMap->erase(clntfd);
    p_container->getServerConnectionMap()->erase(clntfd);

    return;
  }

  if(servfd == 0) { // Receive connect message, or Socket and Event is empty.
    servfd = createSocketEvent();

    for(auto iter = rib.begin(); iter != rib.end(); iter++) {
      if((memcmp(macAddress, iter->getMacAddress(), sizeof(uint8_t) * 6) == 0)
        && (clntfd == iter->getClientFd())) {
        iter->setServerFd(servfd);
        break;
      }
    }

    (*p_connectionMap)[clntfd] = servfd;
    (*p_container->getServerConnectionMap())[clntfd] = this;
  }

  if(data.length() > 0) { // Receive data.
    send(servfd, data.c_str(), data.length(), 0);
  }
}

inline unordered_map<int, struct event*>*
TcpReceiverFace::getSocketEventMap()
{
  return p_socketEventMap;
}

inline string
TcpReceiverFace::getName()
{
  return m_name;
}

inline int
TcpReceiverFace::getType()
{
  return FACE_TCP_RECEIVER;
}

inline unordered_map<int, int>*
TcpReceiverFace::getConnectionMap()
{
  return p_connectionMap;
}
