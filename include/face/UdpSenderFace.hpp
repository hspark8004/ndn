#include <event2/event.h>
#include <event2/util.h>
#include <string>
#include <unordered_map>

using namespace std;

class UdpSenderFace : public Face
{
public:
  UdpSenderFace(string name, int port);
  ~UdpSenderFace();
  Container* getContainer();
  static void onReadSocket(evutil_socket_t fd, short events, void* arg);
  void onReceiveData(char* name, unsigned char* data, size_t size);
  void sendInterest(int fd, char* name, uint64_t len);
  virtual string getName();
  virtual int getType();
private:
  Container* p_container;
  struct event* p_event;
  string m_name;
  int m_servfd;
  int m_port;
};
