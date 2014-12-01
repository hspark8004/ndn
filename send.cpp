// See : "Send an arbitrary Ethernet frame using libpcap"
// http://www.microhowto.info/howto/send_an_arbitrary_ethernet_frame_using_libpcap.html

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcap.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <netinet/if_ether.h>
#include <sys/ioctl.h>

#include <unistd.h>

#include "NdnlpData.h"
#include "Interest.h"

#include <iostream>

using namespace std;


int main(int argc, const char* argv[])
{
    // Get interface name and target IP address from command line.
    const char* if_name = "eth0";
    
    // Construct Ethernet header (except for source MAC address);
    // (Destination set to broadcast address, FF:FF:FF:FF:FF:FF.)

    struct ether_header header;

    header.ether_type = 10000;  // sample value

    memset(header.ether_dhost, 0xff, sizeof(header.ether_dhost));

    

    struct ifreq ifr;
    size_t if_name_len=strlen(if_name);

    if(if_name_len<sizeof(ifr.ifr_name)) {
        memcpy(ifr.ifr_name, if_name, if_name_len);
        ifr.ifr_name[if_name_len]=0;
    } else {
        fprintf(stderr, "interface name is too long");
        exit(1);
    }

    // Open an IPv4-family socket for use when calling ioctl.
    int fd=socket(AF_INET, SOCK_DGRAM, 0);
    if(fd==-1) {
        fprintf(stderr, "fd error\n");
        perror(0);
        exit(1);
    }

    // Obtain the source IP address, copy into ARP request
    if(ioctl(fd, SIOCGIFADDR, &ifr)==-1) {
        fprintf(stderr, "SIOCGIFADDR error\n");
        perror(0);
        close(fd);
        exit(1);
    }
    struct sockaddr_in* source_ip_addr = (struct sockaddr_in*)&ifr.ifr_addr;
//    memcpy(&req.arp_spa, &source_ip_addr->sin_addr.s_addr, sizeof(req.arp_spa));
    
    // Obtain the source MAC address, copy into Ethernet header and ARP request.
    
    if(ioctl(fd, SIOCGIFHWADDR, &ifr)==-1) {
        fprintf(stderr, "SIPCGIFHWADDR error\n");
        perror(0);
        close(fd);
        exit(1);
    }
    if(ifr.ifr_hwaddr.sa_family!=ARPHRD_ETHER) {
        fprintf(stderr, "not an Ethernet interface");
        close(fd);
        exit(1);
    }
    const unsigned char* source_mac_addr = (unsigned char*)ifr.ifr_hwaddr.sa_data;
    memcpy(header.ether_shost, source_mac_addr, sizeof(header.ether_shost));

    header.ether_dhost[0] = 0x00;
    header.ether_dhost[1] = 0x13;
    header.ether_dhost[2] = 0x77;
    header.ether_dhost[3] = 0xcf;
    header.ether_dhost[4] = 0xa0;
    header.ether_dhost[5] = 0x3d;

/*
    printf("%x:", header.ether_dhost[0]);
    printf("%x:", header.ether_dhost[1]);
    printf("%x:", header.ether_dhost[2]);
    printf("%x:", header.ether_dhost[3]);
    printf("%x:", header.ether_dhost[4]);
    printf("%x:\n", header.ether_dhost[5]);
*/
    
    close(fd);


// Open a PCAP packet capture descriptor for the specified interface.
    char pcap_errbuf[PCAP_ERRBUF_SIZE];
    pcap_errbuf[0]='\0';
    // if_name = eth0;
    pcap_t* pcap = pcap_open_live(if_name, 96, 0, 0, pcap_errbuf);
    if(pcap_errbuf[0] != '\0') {
        fprintf(stderr, "%s\n", pcap_errbuf);
    }
    if(!pcap) {
        exit(1);
    }
    

    
    // Combine the Ethernet header and ARP request into a contiguous block.
    unsigned char frame[
        sizeof(struct ether_header)+
        sizeof(struct NdnlpData)+
        sizeof(Interest)];

    memcpy(frame, &header, sizeof(struct ether_header));


    for(int i=0; i<1; i++) {
        struct NdnlpData lp_data_packet_header;
        lp_data_packet_header.SeqNum = i;
        lp_data_packet_header.Flags = 2;
        lp_data_packet_header.FragIndex = 0;
        lp_data_packet_header.FragCount = 10;
        lp_data_packet_header.payload = 100;

        printf("%d\n", lp_data_packet_header.SeqNum);

        memcpy(frame+sizeof(struct ether_header), &lp_data_packet_header, sizeof(struct NdnlpData));


        Interest inte;
        inte.setName("org/secmem/gn/a.mp4\0");
        inte.setSelector(1000);
        inte.setScope(1);
        inte.setTime(10000);

        memcpy(frame+sizeof(struct ether_header) + sizeof(struct NdnlpData), &inte, sizeof(struct Interest));

    // Write the Ethernet frame to the interface.
        if(pcap_inject(pcap, frame, sizeof(frame))==-1) {
            pcap_perror(pcap, 0);
            pcap_close(pcap);
            exit(1);
        }
    }
    
//    memcpy(frame+sizeof(struct ether_header)+sizeof(NdnlpData), &inte, sizeof(Interest));

    


    
    
    // Close the PCAP descriptor.
    
    pcap_close(pcap);
    printf("success Send Ethernet frame\n");

    return 0;
}
