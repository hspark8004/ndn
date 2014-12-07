#include "ReqInformation.hpp"

ReqInformation::ReqInformation()
{
}

ReqInformation::ReqInformation(int index, Interest interest, uint8_t* macAddress)
  : m_index(index)
{
  m_clntfd = interest.extractFileDescriptor();
  memcpy(m_macAddress, macAddress, sizeof(uint8_t) * 6);
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
