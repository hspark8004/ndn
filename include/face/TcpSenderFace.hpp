#include <event2/event.h>
#include <event2/util.h>
#include <string>
#include <unordered_map>
#include <utility>
#include "face/Face.hpp"
#include "Common.hpp"
#include "Interest.hpp"
#include "Data.hpp"

using namespace std;

class TcpSenderFace : public Face
{
public:
  TcpSenderFace(string name, int port);
  ~TcpSenderFace();
  static void onAcceptSocket(evutil_socket_t fd, short events, void* arg);
  static void onReadSocket(evutil_socket_t fd, short events, void* arg);
  void onReceiveData(char* name, unsigned char* data, size_t size);
  void sendInterest(const char* name, size_t len);
  unordered_map<int, struct event*>* getSocketEventMap();
  string getName();
#ifdef __DEBUG_MODE
  void setSendSocket(int fd);
  void setRecvSocket(int fd);
#endif /* __DEBUG_MODE */
private:
  void initSocket();
  void initEvent();
  const int m_backlog = 100;
  struct event* p_event;
  unordered_map<int, struct event*>* p_socketEventMap;
  struct sockaddr_in m_servaddr;
  string m_name;
  socklen_t m_addrlen;
  int m_servfd;
  int m_port;
#ifdef __DEBUG_MODE
  int m_sendSocket;
  int m_recvSocket;
#endif /* __DEBUG_MODE */
};
