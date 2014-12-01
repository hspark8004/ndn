#include <stdio.h>
#include <pcap.h>
#include <stdlib.h>
#include <string.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <netinet/if_ether.h>   // struct ether_header
#include <arpa/inet.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>

#include "Container.hpp"


// argv[1] : comName : ex) org/secmem/gn
int main(int argc, const char* argv[])
{
    if( argc < 2 ) {
        std::cout << "Parameter error!" << std::endl;
        std::cout << "example) ./main org/secmem/gn" << std::endl;
        exit(1);
    }
    char* comName = new char[strlen(argv[1])];
    strcpy(comName, argv[1]);
    
    Container container(comName);

    container.getNdnLayer()->sendInterest(10, "org/secmem/gn/11/a.mp4");
	
    sleep(30000);
}
