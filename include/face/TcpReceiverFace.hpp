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
  int createSocketEvent();
  void removeSocketEvent(int fd);
  static void onReadSocket(evutil_socket_t fd, short events, void* arg);
  void onReceiveInterest(Interest& interest, uint8_t* macAddress);
  void sendData(Data& data);
  unordered_map<int, struct event*>* getSocketEventMap();
  unordered_map<int, int>* getConnectionMap();
  virtual string getName();
  virtual int getType();
  Container* getContainer() { return p_container; };
  uint64_t* getSegmentIndex() { return p_dataIndex; };
  void setSegmentIndex(uint64_t idx) { *p_dataIndex = idx; }
private:
  Container* p_container;
  struct event_base* m_eventBase;
  uint64_t* p_dataIndex;
  unordered_map<int, struct event*>* p_socketEventMap;
  unordered_map<int, int>* p_connectionMap;
  string m_name;
  int m_port;
};
