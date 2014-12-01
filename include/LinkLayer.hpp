#ifndef LINKLAYER_HPP_
    #define LINKLAYER_HPP_

#include <iostream>
#include <unordered_map>
#include <ctime>

#include "Interest.hpp"
#include "Container.hpp"
#include "NdnlpData.hpp"
#include "tlv_type.hpp"
#include "tlv_length.hpp"
#include "Fragment.hpp"
#include "DefineVariable.hpp"

using namespace std;

class Container;
class LinkLayer {
    private : 
        Container* container;
        unordered_map<uint64_t, Fragment*> temporaryStore;
    public : 
        LinkLayer();
        LinkLayer(Container* container);

        Container* getContainer();
        int sendInterest(int fd, unsigned char* data, uint64_t size);
        int sendInterest(Interest interest);
        int sendInterest(tlv_type type, tlv_length length, Interest interest);

        int sendData(int serverFd, unsigned char* data, uint64_t size);

        void recvNdnPacket(unsigned char* packet, uint8_t* shost_mac);

        void constructTempStore(NdnlpData& lp, unsigned char* data, uint8_t* shost_mac);
        void registerTempStore(NdnlpData& lp, unsigned char* data, uint8_t* shost_mac);
        void assemblyTempStore(NdnlpData& lp, unsigned char* data, uint8_t* shost_mac);
};

#endif
