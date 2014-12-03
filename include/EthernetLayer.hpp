#ifndef ETHERNETLAYER_HPP_
    #define ETHERNETLAYER_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <netinet/if_ether.h>   // struct ether_header
#include <arpa/inet.h>
#include <stdint.h>

#include <sys/ioctl.h>

#include <pthread.h>

#include "LinkLayer.hpp"


class Container;
class EthernetLayer {
    private : 
        static pcap_t* pcap_handle;
        Container* container;

        struct pcap_pkthdr header; /* The header that pcap gives us */
        unsigned char* recvPacket; /* The actual packet */
    
        char* spNetDevName;
    public : 
        EthernetLayer();
        EthernetLayer(Container* container);

        Container* getContainer();

        unsigned char* getMacAddress(char* interface_name);
        unsigned char* getMacAddress();

        int sendInterest(unsigned char* data, uint64_t size);   // write to send()
        int sendData(int serverFd, unsigned char* data, uint64_t size);       // write to send()
        //int sendInterest(NdnlpData ndnlp, Interest interest);
        int send(unsigned char* data, uint64_t size);   // write to NIC
        int Initializing_pcap();

        static void* receive(void * arg);
};


#endif
