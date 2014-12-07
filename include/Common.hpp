#include <event2/event.h>
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <list>
#include <vector>
#include "face/Face.hpp"
#include "ReqInformation.hpp"
#include "Data.hpp"

#define FACE_TCP_SENDER 2000
#define FACE_TCP_RECEIVER 2001

using namespace std;

runtime_error runtimeError(int* err);

string getPrefix(string name);
string getData(string name);

string urlEncode(string str);
string urlDecode(string str);

void addInterestInformation(Interest interest, uint8_t* macAddress);
void showInterestInformation();
ReqInformation* getInterestInformation(int servfd);

typedef vector<ReqInformation> rib_t;
extern rib_t rib;

extern struct event_base* eventBase;
extern int NextRecvInterestsIndex;
extern int serverFaceId;
