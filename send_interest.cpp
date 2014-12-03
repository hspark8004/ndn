#include <event2/event.h>
#include <event2/util.h>
#include <event2/thread.h>
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
#include "Common.hpp"
#include "Container.hpp"


// argv[1] : comName : ex) org/secmem/gn
int main(int argc, const char* argv[])
{
    if( argc < 2 ) {
        std::cout << "Parameter error!" << std::endl;
        std::cout << "example) ./main org/secmem/gn" << std::endl;
        exit(1);
    }

    evthread_use_pthreads();
    event_enable_debug_mode();
    eventBase = event_base_new();

    char* comName = new char[strlen(argv[1])];
    strcpy(comName, argv[1]);
    
    Container container(comName);

	event_base_dispatch(eventBase);
}
