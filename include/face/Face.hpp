#ifndef __FACE
#define __FACE
#include <string>

using namespace std;

class Face
{
protected:
  virtual int getType() = 0;
  virtual string getName() = 0;
};
#endif /* __FACE */
