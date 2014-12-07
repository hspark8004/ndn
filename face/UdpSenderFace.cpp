#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include "Common.hpp"

using namespace std;

UdpSenderFace::UdpSenderFace(string name, int port)
  : m_name(name), m_port(port)
{
  struct sockaddr_in servaddr;

  if((m_servfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    throw runtimeError(&errno);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(m_port);

  if(bind(m_servfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
    throw runtimeError(&errno);
  }

  p_event = event_new(eventBase, m_servfd, EV_READ | EV_PERSIST,
    UdpSenderFace::onRead, (void*)this);

  if(event_add(p_event, NULL) < 0) {
    throw runtimeError(&errno);
  }
}

UdpSenderFace::~UdpSenderFace()
{
  event_del(p_event);
  close(m_servfd);
}

inline Container*
UdpSenderFace::getContainer()
{
  return p_container;
}

void
UdpSenderFace::onReadSocket(evutil_socket_t fd, short events, void* arg)
{
  UdpSenderFace* pThis = (UdpSenderFace*)arg;
  struct 
  struct data add_data;
  char buf[BUFSIZ];
  int ret = recvfrom(sockfd, (void*)&add_data, sizeof(add_data), 0, 
}

