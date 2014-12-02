#include <event2/event.h>
#include <event2/util.h>
#include <string>
#include <list>
#include <utility>
#include <unordered_map>
#include "face/Face.hpp"
#include "Common.hpp"
#include "Container.hpp"

using namespace std;

class TcpReceiverFace : public Face
{
public:
  TcpReceiverFace(Container* container, string name, int port);
  ~TcpReceiverFace();
  int createConnection();
  static void onReadSocket(evutil_socket_t fd, short events, void* arg);
  void onReceiveInterest(unsigned char* packet, uint8_t* shost_mac);
  unordered_map<int, struct event*>* getSocketEventMap();
  virtual string getName();
  virtual int getType();
#ifdef __DEBUG_MODE
  static void onTest(evutil_socket_t fd, short events, void* arg);
  void setSendSocket(int fd);
  void setRecvSocket(int fd);
  int getRecvSocket();
#endif /* __DEBUG_MODE */
private:
  Container* p_container;
  struct event_base* m_eventBase;
  unordered_map<int, struct event*>* p_socketEventMap;
  unordered_map<int, int> m_connectionMap;
  string m_name;
  int m_port;
#ifdef __DEBUG_MODE
  int m_sendSocket;
  int m_recvSocket;
#endif /* __DEBUG_MODE */
};
