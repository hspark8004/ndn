#ifndef NDNLAYER_HPP_
#define NDNLAYER_HPP_

#include "Container.hpp"
#include "Interest.hpp"
#include "Data.hpp"
#include "tlv_length.hpp"

class Container;
class NdnLayer
{
public: 
  NdnLayer(Container* container);
  void sendInterest(Interest& interest);
  void sendData(Data& data);
  void recvInterestPacket(unsigned char* packet, tlv_length length, uint8_t* macAddress);
  void recvDataPacket(unsigned char* packet, tlv_length length);
  void recvNdnPacket(unsigned char* packet, uint8_t* macAddress);
private: 
  Container* container;
};
#endif /* NDNLAYER_HPP_ */
