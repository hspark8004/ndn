#include "NdnLayer.hpp"
#include "Common.hpp"
#include "face/TcpReceiverFace.hpp"
#include "face/TcpSenderFace.hpp"

using namespace std;

NdnLayer::NdnLayer(Container* container)
{
  cout << "NdnLayer(Container& container)" << endl;

  this->container = container;
}

void
NdnLayer::sendInterest(Interest& interest)
{
  int fd = stoi(getData(getPrefix(interest.getName())));
  tlv_type type(1); // 나중에 #define 으로 정의하기!
  tlv_length length(interest.getNameSize(), 0);

  unsigned char buf[sizeof(type) + sizeof(length) + interest.size()];
  memcpy(buf, &type, sizeof(type));
  memcpy(buf + sizeof(type), &length, sizeof(length));
  memcpy(buf + sizeof(type) + sizeof(length), interest.getByte(), interest.size());
  
  container->getLinkLayer()->sendInterest(fd, buf, sizeof(buf));
}

void
NdnLayer::recvNdnPacket(unsigned char* packet, uint8_t* macAddress)
{
  cout << "void NdnLayer::recvNdnPacket(unsigned char* packet)" << endl;

  tlv_type type;
  tlv_length length;

  memcpy(&type, packet, sizeof(type));
  memcpy(&length, packet + sizeof(type), sizeof(length));

  switch((unsigned int)type.getTlvType())
  {
  case 1: // Interest
    recvInterestPacket(packet + sizeof(type) + sizeof(length), length, macAddress);
    break;
  case 2: // Data
    recvDataPacket(packet + sizeof(type) + sizeof(length), length);
    break;
  default: // Error
    break;
  }
}

void
NdnLayer::recvInterestPacket(unsigned char* name, tlv_length length, uint8_t* macAddress)
{
  cout << __LINE__ << ": NdnLayer::recvInterestPacket" << endl;

  Interest interest;
  interest.setNameSize(length.getNameLength());
  interest.setInterest(name, length.getNameLength());
  interest.showInterestData();

  addInterestInformation(interest, macAddress);
  showInterestInformation();

  auto serverMap = container->getServerConnectionMap();

  for(auto iter = serverMap->begin(); iter != serverMap->end(); iter++) {
    TcpReceiverFace* face = (TcpReceiverFace*)iter->second;

    if(face->getName().compare(interest.getName()) == 0) {
      face->onReceiveInterest(interest, macAddress);
      return;
    }
  }
}

void
NdnLayer::recvDataPacket(unsigned char* packet, tlv_length length)
{
  cout << __LINE__ << ": NdnLayer::recvDataPacket" << endl;

  Data data;
  data.setData(packet, length);
  data.showData();

  auto clientMap = container->getClientConnectionMap();
  auto face = clientMap->find(data.extractAppName());

  if(face == clientMap->end()) {
      cout << "Not Found client fd" << endl;
      return;
  }

  ((TcpSenderFace*)(face->second))->onReceiveData(data);
}

void
NdnLayer::sendData(Data& data)
{
  int clntfd = data.extractAppName();
  int servfd = 0;

  for(int i = 0; i < rib.size(); i++) {
    if(clntfd == rib[i].getClientFd()) {
      servfd = rib[i].getServerFd();
      break;
    }
  }

  if(servfd == 0) {
    cout << __LINE__ << ": 데이터 패킷을 보낼 관심 패킷의 정보가 없습니다." << endl;
    return;
  }

  tlv_type type(2);
  tlv_length length(data.getNameSize(), data.getContentSize());

  unsigned char buf[sizeof(type) + sizeof(length) + data.getSize()];

  memcpy(buf, &type, sizeof(type));
  memcpy(buf + sizeof(type), &length, sizeof(length));
  memcpy(buf + sizeof(type) + sizeof(length), data.getByte(), data.getSize());
  
  container->getLinkLayer()->sendData(servfd, buf, sizeof(buf));
}
