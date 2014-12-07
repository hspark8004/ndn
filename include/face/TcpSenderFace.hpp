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

typedef tuple<uint64_t, unsigned char*, size_t> segment_t; // segment, buffer, length

class TcpSenderFace : public Face
{
public:
  TcpSenderFace(Container* container, string name, int port);
  ~TcpSenderFace();
  static void onAcceptSocket(evutil_socket_t fd, short events, void* arg);
  static void onReadSocket(evutil_socket_t fd, short events, void* arg);
  void onReceiveData(Data& data);
  void sendInterest(Interest& interest);
  unordered_map<int, struct event*>* getSocketEventMap();
  Container* getContainer() { return p_container; }
  vector<segment_t> getSegmentList();
  uint64_t getSegmentIndex();
  virtual string getName();
  virtual int getType();
private:
  const int m_backlog = 100;
  Container* p_container;
  struct event* p_event;
  unordered_map<int, struct event*>* p_socketEventMap;
  vector<segment_t> m_segmentList;
  uint64_t m_segmentIndex = 1;
  string m_name;
  int m_servfd;
  int m_port;
};
