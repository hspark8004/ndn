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

TcpReceiverFace::TcpReceiverFace(Container* container, string name, int port)
  : p_container(container), m_name(name), m_port(port)
{
  p_socketEventMap = new unordered_map<int, struct event*>;
  p_connectionMap = new unordered_map<int, int>;

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
TcpReceiverFace::createConnection()
{
  struct event* e;
  struct sockaddr_in servaddr;
  socklen_t addrlen = sizeof(servaddr);
  int clntfd;
  int ret;

  if((clntfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    throw runtimeError(&errno);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(m_port);

  if((ret = connect(clntfd, (struct sockaddr*)&servaddr, addrlen)) < 0) {
    throw runtimeError(&errno);
    // return -1; // Fail to connect legacy server.
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
TcpReceiverFace::onReadSocket(evutil_socket_t fd, short events, void* arg)
{
  TcpReceiverFace* pThis = (TcpReceiverFace*)arg;
  char buf[BUFSIZ];
  ssize_t len = recv(fd, buf, BUFSIZ, 0);
  string data(buf, len);
  string prefix(pThis->getName());

  sprintf(buf, "%d", fd); // Get file descriptor.
  prefix.append("/").append(buf); // Append file descriptor.

  cout << "READ!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;

  if(len == 0) {
    auto socketEventMap = pThis->getSocketEventMap();

    event_free((*socketEventMap)[fd]);
    close(fd);
    socketEventMap->erase(fd);

    return;
  }

  int clntfd = 0;
  auto connMap = pThis->getConnectionMap();

  for(auto iter = connMap->begin(); iter != connMap->end(); iter++) {
    if(fd == iter->second) {
      clntfd = iter->first;
      break;
    }
  }

  if(clntfd == 0) {
    cout << "No mapping socket..." << endl;
    return;
  }

  string s_name(pThis->getName());
  len = sprintf(buf, "%d", clntfd);
  s_name = s_name.append("/").append(buf, len);

  char* name = new char[s_name.length() + 1];
  copy(s_name.begin(), s_name.end(), name);
  name[s_name.length()] = 0;

  unsigned char* s_data = new unsigned char[data.length() + 1];
  copy(data.begin(), data.end(), s_data);
  s_data[data.length()] = 0;

  Data DataStruct(name, s_data, data.length());

  std::cout << "Data content Size : " << DataStruct.getContentSize() << std::endl;
  std::cout << "Data Name : " << DataStruct.getName() << std::endl;
  std::cout << "Data NameSize : " << DataStruct.getNameSize() << std::endl;

  DataStruct.showData();

  tlv_length tlvLength(DataStruct.getNameSize(), DataStruct.getContentSize());
  tlv_type tlvType(2);

  unsigned char buffer[sizeof(tlv_type) + sizeof(tlv_length) + DataStruct.getSize()];

  memcpy(buffer, &tlvType, sizeof(tlv_type));
  memcpy(buffer + sizeof(tlv_type), &tlvLength, sizeof(tlv_length));
  memcpy(buffer + sizeof(tlv_type) + sizeof(tlv_length), DataStruct.getByte(), DataStruct.getSize());
    
  pThis->getContainer()->getLinkLayer()->sendData(clntfd, buffer, sizeof(buffer));
}

void
TcpReceiverFace::onReceiveInterest(int clntfd, string data, uint8_t* shost_mac)
{
  cout << "DDDDDDDDDData: " << data << endl;
  int servfd = (*p_connectionMap)[clntfd];

  cout << "RECV: " << servfd << endl;

  if(data.length() == 0) {
    cout << "RECV: data length =0" << servfd << endl;
    if(servfd > 0) {
      cout << "RECV:  close gfogogogogo" << servfd << endl;
      p_connectionMap->erase(clntfd);
      close(servfd);
    }
  } else {
    cout << "RECV: datalength: " << data.length() << endl;
    cout << "SERV: " << servfd << endl;

    if(servfd == 0) {
      servfd = createConnection();

      cout << "RECV: datalewersefwaefawefaewfangth: " << servfd << endl;

      for(auto iter = rib.begin(); iter != rib.end(); iter++) {
        auto info = iter;

        if((*shost_mac == *info->getMacAddress())
          && (clntfd == info->getClientFd())) {
          info->setServerFd(servfd);
          break;
        }
      }

      (*p_connectionMap)[clntfd] = servfd;
      (*p_container->getServerConnectionMap())[clntfd] = this;
    }

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
