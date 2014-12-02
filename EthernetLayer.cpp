#include "EthernetLayer.hpp"
#include "Common.hpp"
#include <errno.h>

pcap_t* EthernetLayer::pcap_handle = NULL;

EthernetLayer::EthernetLayer() {
    std::cout << "EthernetLayer Constructor" << std::endl;

    Initializing_pcap();
}

EthernetLayer::EthernetLayer(Container* container) {
    std::cout << "EthernetLayer(Container* container)" << std::endl;
    this->container = container;

    Initializing_pcap();

    pthread_t p_thread;
    int thread_id;
    thread_id = pthread_create( &p_thread, NULL, EthernetLayer::receive, this );
   
}

Container*
EthernetLayer::getContainer() {
    return container;
}

int
EthernetLayer::Initializing_pcap() {
    std::cout << "ethernetLayer::Initializing_pcap()" << std::endl;

    if( pcap_handle == NULL )
    {
       char pcap_errbuf[PCAP_ERRBUF_SIZE];
       pcap_errbuf[0] = '\0';

       pcap_handle = pcap_open_live("eth0", BUFSIZ, 0,1000, pcap_errbuf);

       if(pcap_errbuf[0] != '\0') {
          fprintf(stderr, "%s\n", pcap_errbuf);
          return -1;
       }
       if(!pcap_handle) {
         fprintf(stderr, "pcap is not true! maybe null..?\n");
         exit(1);
       }
    }
}


int
EthernetLayer::sendInterest(unsigned char* data, uint64_t size) {
    std::cout << "EthernetLayer::sendInterest(unsigned char* buf)" << std::endl;

    struct ether_header ether;

    ether.ether_type = NDN;
    
    const unsigned char* source_mac_addr = getMacAddress("eth0");
    memcpy(ether.ether_shost, source_mac_addr, sizeof(ether.ether_shost));
    memset(ether.ether_dhost, 0xff, sizeof(ether.ether_dhost));

    unsigned char buf[sizeof(ether_header) + size];
    memcpy(buf, &ether, sizeof(ether_header));
    memcpy(buf + sizeof(ether_header), data, size);

    return send(buf, sizeof(buf));
}

int
EthernetLayer::send(unsigned char* data, uint64_t size) {

    std::cout << "EthernetLayer::send(unsigned char* data, uint64_t size)" << std::endl;
    int writeCount = pcap_inject(pcap_handle, data, size);

    if( writeCount == -1 ) {
        fprintf(stderr, "pcap_inject error!\n");
        return -1;
    } else {
        std::cout << "complete packet send using pcap!" << std::endl;
        if( size != writeCount ) {
            std::cout << "===== pcap_inject writeCount mismatching =====\n" << std::endl;
            std::cout << "packet size : " << size << std::endl;
            std::cout << "writeCount  : " << writeCount << std::endl;
            std::cout << "==============================================\n" << std::endl;

        }
    }
}
unsigned char*
EthernetLayer::getMacAddress(char* interface_name)
{
    struct ifreq ifr;
    size_t if_name_len = strlen(interface_name);

    if(if_name_len < sizeof(ifr.ifr_name)) {
        memcpy(ifr.ifr_name, interface_name, if_name_len);
        ifr.ifr_name[if_name_len] = 0;
    } else {
        fprintf(stderr, "interface name is too long\n");
        exit(1);
    }

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd == -1) {
        fprintf(stderr, "fd error\n");
        perror(0);
        exit(1);
    }

    if(ioctl(fd, SIOCGIFADDR, &ifr) == -1) {
        fprintf(stderr, "SIOCGIFADDR error\n");
        perror(0);
        close(fd);
        exit(1);
    }

    if(ioctl(fd, SIOCGIFHWADDR, &ifr) == -1) {
      perror("ioctl");
        fprintf(stderr, "SIOCGIFHWADDR error\n");
        perror(0);
        close(fd);
        exit(1);
    }

    if(ifr.ifr_hwaddr.sa_family != ARPHRD_ETHER) {
        fprintf(stderr, "not an Ethernet interface");
        close(fd);
        exit(1);
    }
    
    unsigned char* ret = (unsigned char*)ifr.ifr_hwaddr.sa_data;
    close(fd);
  return ret;
}

void*
EthernetLayer::receive(void * arg) {
    EthernetLayer* pThis = ((EthernetLayer*)arg);
    printf("\n  [Thread] receive Thread is create \n");
    
   	//sleep(5000);
	printf("------------ try receive ---------------\n");


    while(1) {
	    ether_header ehP;
        pThis->recvPacket = (unsigned char*)pcap_next(pThis->pcap_handle, &pThis->header);
    if(pThis->recvPacket == NULL) continue;
		memcpy(&ehP, pThis->recvPacket, sizeof(ether_header));

        switch(ehP.ether_type)
        {
            case NDN : 
                 std::cout << "Receive NDN" << std::endl;
                pThis->getContainer()->getLinkLayer()->recvNdnPacket(pThis->recvPacket + sizeof(ether_header), ehP.ether_shost);
                break;
        }
    }

}


int
EthernetLayer::sendData(int serverFd, unsigned char* data, uint64_t size) 
{
    std::cout << "int EthernetLayer::sendData(unsigned char* data, uint64_t size)" << std::endl;

    struct ether_header ether;
    ether.ether_type = NDN;

    const unsigned char* source_mac_addr = getMacAddress("wlan0");
    memcpy(ether.ether_shost, source_mac_addr, sizeof(ether.ether_shost));

    ReqInformation* req = getInterestInformation(serverFd);
    if( req == NULL ) {
        std::cout << "Not Correct Information : " << serverFd << std::endl;
        return -1;
    } else {
        memcpy(ether.ether_dhost, req->getMacAddress(), sizeof(ether.ether_dhost));

        unsigned char buf[sizeof(ether_header) + size];
        memcpy(buf, &ether, sizeof(ether_header));
        memcpy(buf + sizeof(ether_header), data, size);

        send(buf, sizeof(buf));
    }
}
