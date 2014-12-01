#ifndef NDNLAYER_HPP_
    #define NDNLAYER_HPP_

#include "Container.hpp"
#include "Interest.hpp"
#include "tlv_length.hpp"
#include "Data.hpp"
class Container;
class NdnLayer {
    private : 
        Container* container;

    public : 
        NdnLayer();
        NdnLayer(Container* container);

        void setContainer(Container* con);

        Container* getContainer();
        void sendInterest(int fd, char* name);   // send data to Link Layer

        void recvNdnPacket(unsigned char* packet, uint8_t* shost_mac);
        void recvInterestPacket(unsigned char* packet, tlv_length length, uint8_t* shost_mac);
        void recvDataPacket(unsigned char* packet, tlv_length length);

        bool isCorrectDest(Interest interest);
        void sendData(int serverFd, unsigned char* buf, uint64_t size);

        char* combineCharAndInteger(char* c1, int num);
        char* combineChars(char* c1, char* c2);
};

#endif
