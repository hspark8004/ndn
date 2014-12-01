#include <string.h>
#include <stdexcept>
#include <string>
#include "Common.hpp"
#include "face/Face.hpp"
#include "FaceInformationBase.hpp"

using namespace std;

pit_t pit;
fib_t fib;

static char
htoc(string s)
{
  char high = s[0];
  char low = s[1];

  if(high >= 48 && high <= 57) {
    high -= 48;
  } else if(high >= 65 && high <= 90) {
    high -= 65 - 10;
  }

  if(low >= 48 && low <= 57) {
    low -= 48;
  } else if(low >= 65 && low <= 90) {
    low -= 65 - 10;
  }

  char ret = 0;
  ret |= (high << 4);
  ret |= low;

  return ret;
}

static string
ctoh(char c)
{
  char high = (c & 0xF0) >> 4;
  char low = (c & 0x0F);

  if(high >= 0 && high <= 9) {
    high += 48; // '0' == 48
  } else if(high >= 10 && high <= 15) {
    high += 65 - 10; // 'A' == 65
  }

  if(low >= 0 && low <= 9) {
    low += 48;
  } else if(low >= 10 && low <= 15) {
    low += 65 - 10;
  }

  string ret;
  ret += high;
  ret += low;

  return ret;
}

runtime_error runtimeError(int* err)
{
  runtime_error ex = runtime_error(strerror(*err));
  *err = 0;
  return ex;
}

string getPrefix(string name)
{
  size_t end = 0;
  end = name.rfind("/");

  return name.substr(0, end);
}

string getData(string name)
{
  size_t pos = 0;
  pos = name.rfind("/");

  return name.substr(pos + 1, name.length() - pos);
}

string urlEncode(string url)
{
  string ret;

  for(int i = 0; i < url.length(); i++) {
    char c = url[i];

    if((c >= 0x30 && c <= 0x39) // DIGIT
      || (c >= 0x41 && c <= 0x5A) // ALPHA, BIG
      || (c >= 0X61 && c <= 0x7A) // ALPHA, SMALL
      || (c == 0x2D || c == 0x2E || c == 0x5F || c == 0x7E)) { // - . _ ~
      ret += c;
    } else {
      ret += '%';
      ret += ctoh(c);
    }
  }

  return ret;
}

string urlDecode(string url)
{
  string ret;

  for(int i = 0; i < url.length(); i++) {
    if(url[i] == '%') {
      ret += htoc(url.substr(i + 1, 2));
      i += 2;
    } else {
      ret += url[i];
    }
  }

  return ret;
}
