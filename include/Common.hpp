#include <event2/event.h>
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <list>
#include "face/Face.hpp"

#define __DEBUG_MODE

#define FACE_TCP_SENDER 0
#define FACE_TCP_RECEIVER 1

using namespace std;

runtime_error runtimeError(int* err);

string getPrefix(string name);
string getData(string name);

string urlEncode(string str);
string urlDecode(string str);

extern struct event_base* eventBase;
