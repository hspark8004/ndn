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

inline void
ReqInformation::setMacAddress(uint8_t* macAddress)
{
  memcpy(m_macAddress, macAddress, sizeof(uint8_t) * 6);
}

inline uint8_t*
ReqInformation::getMacAddress()
{
    return m_macAddress;
}

inline void
ReqInformation::setIndex(int index)
{
  m_index = index;
}

inline int
ReqInformation::getIndex()
{
  return m_index;
}

inline void
ReqInformation::setClientFd(int fd)
{
  m_clntfd = fd;
}

inline int
ReqInformation::getClientFd()
{
  return m_clntfd;
}

inline void
ReqInformation::setServerFd(int fd)
{
  m_servfd = fd;
}

inline int
ReqInformation::getServerFd()
{
  return m_servfd;
}
#endif /* REQINFORMATION_HPP_ */
