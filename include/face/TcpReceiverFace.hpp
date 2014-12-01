#include <event2/event.h>
#include <event2/util.h>
#include <string>
#include <list>
#include <utility>
#include <unordered_map>
#include "face/Face.hpp"
#include "Common.hpp"

using namespace std;

class TcpReceiverFace : public Face
{
public:
  TcpReceiverFace(string name, int port);
  ~TcpReceiverFace();
  int createConnection();
  static void onReadSocket(evutil_socket_t fd, short events, void* arg);
  void onReceiveInterest(char* interest);
  unordered_map<int, struct event*>* getSocketEventMap();
#ifdef __DEBUG_MODE
  static void onTest(evutil_socket_t fd, short events, void* arg);
  void setSendSocket(int fd);
  void setRecvSocket(int fd);
  int getRecvSocket();
#endif /* __DEBUG_MODE */
private:
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