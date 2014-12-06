#include <event2/event.h>
#include <event2/util.h>
#include <string>
#include <unordered_map>
#include <utility>
#include <tuple>
#include "face/Face.hpp"
#include "Common.hpp"
#include "Interest.hpp"
#include "Data.hpp"
#include "Container.hpp"

using namespace std;

class TcpSenderFace : public Face
{
public:
  TcpSenderFace(Container* container, string name, int port);
  ~TcpSenderFace();
  static void onAcceptSocket(evutil_socket_t fd, short events, void* arg);
  static void onReadSocket(evutil_socket_t fd, short events, void* arg);
  void onReceiveData(char* name, unsigned char* data, size_t size);
  void sendInterest(int fd, char* name, uint64_t len);
  unordered_map<int, struct event*>* getSocketEventMap();
  Container* getContainer() { return p_container; }
  vector<tuple<uint64_t, unsigned char*, size_t>> getDataList();
  uint64_t getDataIndex();
  virtual string getName();
  virtual int getType();
#ifdef __DEBUG_MODE
  void setSendSocket(int fd);
  void setRecvSocket(int fd);
#endif /* __DEBUG_MODE */
private:
  const int m_backlog = 100;
  Container* p_container;
  struct event* p_event;
  unordered_map<int, struct event*>* p_socketEventMap;
  vector<tuple<uint64_t, unsigned char*, size_t>> m_dataList;
  uint64_t m_dataIndex = 1;
  string m_name;
  int m_servfd;
  int m_port;
#ifdef __DEBUG_MODE
  int m_sendSocket;
  int m_recvSocket;
#endif /* __DEBUG_MODE */
};
