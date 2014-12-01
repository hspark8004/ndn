#ifndef REQINFORMATION_HPP_
    #define REQINFORMATION_HPP_

#include <iostream>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include "Interest.hpp"

class ReqInformation {
private : 
    int idx;
    uint8_t shost[6];
    int clientFileDescriptor;
    int serverFileDescriptor;

public : 
    ReqInformation();
    ReqInformation(int index, Interest interest, uint8_t* shost_mac);

//    void setName(char* name);
    void setIndex(int index);
    void setMacAddress(uint8_t* _shost);
    void setClientFd(int fd);
    void setServerFd(int fd);
//  char* getName();
    int getIndex();
    uint8_t* getMacAddress();
    int getClientFd();
    int getServerFd();
    void showInformation();
};

#endif
