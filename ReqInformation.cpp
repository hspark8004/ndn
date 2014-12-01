#include "ReqInformation.hpp"

ReqInformation::ReqInformation() {

}

ReqInformation::ReqInformation(int index, Interest interest, uint8_t* shost_mac) {
    idx = index;    // index
    setMacAddress(shost_mac);   // src mac-address
    setClientFd(interest.extractFileDescriptor());
    setServerFd(123);
}

void
ReqInformation::setMacAddress(uint8_t* _shost) {
    memcpy(shost, _shost, sizeof(shost));
}

uint8_t*
ReqInformation::getMacAddress() {
    return shost;
}

void
ReqInformation::setIndex(int index) {
    idx = index;
}

int
ReqInformation::getIndex() {
    return idx;
}

void
ReqInformation::setClientFd(int fd) {
    clientFileDescriptor = fd;
}

int
ReqInformation::getClientFd() {
    return clientFileDescriptor;
}

void
ReqInformation::setServerFd(int fd) {
    serverFileDescriptor = fd;
}

int
ReqInformation::getServerFd() {
    return serverFileDescriptor;
}

void
ReqInformation::showInformation() {
    std::cout << "----------" << std::endl;
//    std::cout << "reqName : " << getName() << "\t" << "," << "srcMac : ";
    printf("idx\t| MAC Address\t| clientFd\t| serverFd\n");
    printf("%d\t", getIndex());

    for(int i=0; i<6; i++)
        printf("%x:", getMacAddress()[i]);
    printf("\t");

    //printf("%x:", shost[0]);
    //printf("%x:", shost[1]);
    //printf("%x:", shost[2]);
    //printf("%x:", shost[3]);
    //printf("%x:", shost[4]);
    //printf("%x\n", shost[5]);
    printf("%d\t", getClientFd());
    printf("%d\n", getServerFd());
    std::cout << "----------" << std::endl;
}
