#include "ReqInformation.hpp"

ReqInformation::ReqInformation()
{
}

ReqInformation::ReqInformation(int index, Interest interest, uint8_t* macAddress)
  : m_index(index), m_macAddress(macAddress)
{
  m_clntfd = interest.extractFileDescriptor();
}

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

void
ReqInformation::showInformation()
{
  std::cout << "----------" << std::endl;
//    std::cout << "reqName : " << getName() << "\t" << "," << "srcMac : ";
  printf("idx\t| MAC Address\t| clientFd\t| serverFd\n");
  printf("%d\t", getIndex());

  for(int i=0; i<6; i++)
      printf("%x:", getMacAddress()[i]);
  printf("\t");

  //printf("%x:", shost[0]);
  //printf("%x:", shost[1]);
  //printf("%x:", shost[2]);
  //printf("%x:", shost[3]);
  //printf("%x:", shost[4]);
  //printf("%x\n", shost[5]);
  printf("%d\t", getClientFd());
  printf("%d\n", getServerFd());
  std::cout << "----------" << std::endl;
}
