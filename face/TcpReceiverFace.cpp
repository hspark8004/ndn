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
#include "Container.hpp"

using namespace std;

static pair<unsigned char*, int>
parseData(Data* DataStruct)
{
  unsigned char* buf;
  int len;
  tlv_length tlvLength(DataStruct->getNameSize(), DataStruct->getContentSize());
  tlv_type tlvType(2);

  len = sizeof(tlv_type) + sizeof(tlv_length) + DataStruct->getSize();

  buf = new unsigned char[len + 1];

  memcpy(buf, &tlvType, sizeof(tlv_type));
  memcpy(buf + sizeof(tlv_type), &tlvLength, sizeof(tlv_length));
  memcpy(buf + sizeof(tlv_type) + sizeof(tlv_length), DataStruct->getByte(),
    DataStruct->getSize());

  buf[len] = 0;

  return pair<unsigned char*, int>(buf, len);
}

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
  ssize_t len;
  int clntfd;

  if((len = recv(fd, buf, BUFSIZ, 0)) == 0) {
    pThis->removeSocketEvent(fd);
    return;
  }

  for(auto iter = p_connectionMap->begin(); iter != p_connectionMap->end(); iter++) {
    if(fd == iter->second) {
      clntfd = iter->first;

      uint64_t index = *pThis->getDataIndex();
      pThis->setDataIndex(index + 1);

      string name = pThis->getName().append("/").append(to_string(clntfd)).append("/").append(to_string(index));
      string data = string(buf, len);

      Data dataPacket(const_cast<char*>(name.c_str()),
        reinterpret_cast<unsigned char*>(const_cast<char*>(data.c_str())), data.length());

      pair<unsigned char*, int> ret = parseData(&dataPacket);
      pThis->getContainer()->getLinkLayer()->sendData(clntfd, ret.first, ret.second);

      return;
    }
  }
}

void
TcpReceiverFace::onReceiveInterest(int clntfd, string data, uint8_t* shost_mac)
{
  int servfd = (*p_connectionMap)[clntfd];

  if((data.length() == 0) && (servfd > 0)) { // Receive close message and socket exists.
    removeSocketEvent(servfd);

    for(auto iter = rib.begin(); iter != rib.end(); iter++) {
      if((*shost_mac == *iter->getMacAddress()) && (clntfd == iter->getClientFd())) {
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
      if((*shost_mac == *iter->getMacAddress()) && (clntfd == iter->getClientFd())) {
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
#endif /* __DEBUG_MODE */
