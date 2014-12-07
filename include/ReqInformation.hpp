#ifndef REQINFORMATION_HPP_
#define REQINFORMATION_HPP_

#include <string>
#include "Interest.hpp"
#include "face/Face.hpp"

using namespace std;

class ReqInformation
{
public:
  ReqInformation();
  ReqInformation(int index, Interest interest, uint8_t* macAddress);
  void setIndex(int index);
  void setMacAddress(uint8_t* macAddress);
  void setClientFd(int fd);
  void setServerFd(int fd);
  int getIndex();
  uint8_t* getMacAddress();
  int getClientFd();
  int getServerFd();
  void showInformation();
  string getName();
  void setName(string name);
  Face* getFace();
  void setFace(Face& face);
private:
  int m_index;
  uint8_t m_macAddress[6];
  int m_clntfd;
  int m_servfd = 0;
  string m_name;
  Face* p_face;
};
#endif /* REQINFORMATION_HPP_ */
