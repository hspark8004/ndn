#include <event2/event.h>
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <list>
#include "face/Face.hpp"
#include "FaceInformationBase.hpp"
#include "PendingInterest.hpp"

#define __DEBUG_MODE

using namespace std;

runtime_error runtimeError(int* err);

string getPrefix(string name);
string getData(string name);

string urlEncode(string str);
string urlDecode(string str);

extern struct event_base* eventBase;

// typedef unordered_map<int, PendingInterest*> pit_t;
typedef list<PendingInterest*> pit_t;
typedef FaceInformationBase fib_t;

extern pit_t pit;
extern fib_t fib;
