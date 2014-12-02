#include <event2/event.h>
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <list>
#include <vector>
#include "face/Face.hpp"
#include "ReqInformation.hpp"

//#define __DEBUG_MODE

#define FACE_TCP_SENDER 2000
#define FACE_TCP_RECEIVER 2001

using namespace std;

runtime_error runtimeError(int* err);

string getPrefix(string name);
string getData(string name);

string urlEncode(string str);
string urlDecode(string str);

void addInterestInformation(Interest interest, uint8_t* shost_mac);
void showInterestInformation();
ReqInformation* getInterestInformation(int serverFd);

extern struct event_base* eventBase;
typedef vector<ReqInformation> rib_t;
extern rib_t rib;

extern int NextRecvInterestsIndex;
