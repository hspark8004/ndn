#include "NdnLayer.hpp"
#include "Common.hpp"


NdnLayer::NdnLayer() {
    std::cout << "NdnLayer Constructor" << std::endl;
}

NdnLayer::NdnLayer(Container* container) {
    std::cout << "NdnLayer(Container& container)" << std::endl;

    this->container = container;
}

Container*
NdnLayer::getContainer() {
    return container;
}

void
NdnLayer::setContainer(Container* con) {
    container = con;
}

void
NdnLayer::sendInterest(int fd, char* name) {

    Interest interest;

    interest.setName(name);
    interest.setSelector(255);
    interest.setScope(1);
    interest.setTime(1000);

    interest.showInterestData();
    tlv_type tlvType(1);

    tlv_length tlvLength(interest.getNameSize(), 0);

    unsigned char buffer[
        sizeof(tlv_type) + 
        sizeof(tlv_length) + 
        interest.size()];

    memcpy(buffer, &tlvType, sizeof(tlv_type));
    memcpy(buffer + sizeof(tlv_type), &tlvLength, sizeof(tlv_length));
    memcpy(buffer + sizeof(tlv_type) + sizeof(tlv_length), interest.getByte(), interest.size());
    
    getContainer()->getLinkLayer()->sendInterest(fd, buffer, sizeof(buffer));

    // 메모리 해제

}

void
NdnLayer::recvNdnPacket(unsigned char* packet, uint8_t* shost_mac) {
    std::cout << "void NdnLayer::recvNdnPacket(unsigned char* packet)" << std::endl;

    tlv_type type;
    tlv_length packet_size;

    memcpy(&type, packet, sizeof(tlv_type));
    memcpy(&packet_size, packet + sizeof(tlv_type), sizeof(tlv_length));

    switch((unsigned int)type.getTlvType()) {
        case 1 : 
            // Interest
            recvInterestPacket(packet + sizeof(tlv_type) + sizeof(tlv_length), packet_size, shost_mac);
            break;
        case 2 : 
            // Data
            recvDataPacket(packet + sizeof(tlv_type) + sizeof(tlv_length), packet_size);
            break;
        default : 
            // error
            break;
    }
}

void
NdnLayer::recvInterestPacket(unsigned char* packet, tlv_length length, uint8_t* shost_mac) {
    std::cout << "void NdnLayer::recvInterestPacket(unsigned char* packet, tlv_length length)" << std::endl;

    Interest recvInterest;
    recvInterest.setNameSize(length.getNameLength());

    recvInterest.setInterest(packet, length.getNameLength());

    recvInterest.showInterestData();
    
    //std::cout << "extract Data : " << recvInterest.extractData() << std::endl;

    addInterestInformation(recvInterest, shost_mac);
    showInterestInformation();

    if(isCorrectDest(recvInterest)) {
        std::cout << "CorrectDest" << std::endl;
        unsigned char tempBuffer[5000];
        memset(tempBuffer, '1', sizeof(tempBuffer));

        std::cout << "before" << std::endl;
        std::cout << tempBuffer << std::endl;
        sendData(123, /* server Fd */
                tempBuffer,
                sizeof(tempBuffer));

    } else {
        std::cout << "Not Correct Dest" << std::endl;
    }
}


void
NdnLayer::recvDataPacket(unsigned char* packet, tlv_length length) {
    std::cout << "void NdnLayer::recvDataPacket(unsigned char* packet, tlv_length length)" << std::endl;

    Data recvData;
    recvData.setData(packet, length);

    recvData.showData();

    std::cout << "data name : " << recvData.getName() << std::endl;
    std::cout << "data com name : " << recvData.extractComName() << std::endl;
    std::cout << "data dest fd : " << recvData.extractAppName() << std::endl;
}

bool
NdnLayer::isCorrectDest(Interest interest) {
    if(!strcmp(interest.extractComName(), getContainer()->getComName())) {
        return true;
    } else {
        return false;
    }
}

void
NdnLayer::sendData(int serverFd, unsigned char* buf, uint64_t size) {

    std::cout << "void NdnLayer::sendData(char* appName, unsigned char* buf, uint64_t size)" << std::endl;

    std::cout << "comName : " << getContainer()->getComName() << std::endl;
    std::cout << "comNameLength : " << strlen(getContainer()->getComName()) << std::endl;
    std::cout << "appName : " << serverFd << std::endl;
    //std::cout << "appNameLength : " << strlen(appName) << std::endl;
    std::cout << "buf length : " << size << std::endl;

    //Data DataStruct(combineChars(getContainer()->getComName(), appName), buf, size);

    int destFd = -1;
    for(int i=0; i< rib.size(); i++) {
        if(serverFd == rib.at(i).getServerFd())
        {
            destFd = rib.at(i).getClientFd();
            break;
        }
    }
    if(destFd == -1) {
        std::cout << "ServerFd " << serverFd << "가 recvInterests에 존재하지 않습니다." << std::endl;
        return;
    }

    char* combineChar = combineCharAndInteger(getContainer()->getComName(), destFd);

    Data DataStruct(combineChar, buf, size);
    
    delete combineChar; // 메모리관리를 위해서....
   

    std::cout << "Data content Size : " << DataStruct.getContentSize() << std::endl;
    std::cout << "Data Name : " << DataStruct.getName() << std::endl;
    std::cout << "Data NameSize : " << DataStruct.getNameSize() << std::endl;

    DataStruct.showData();


    tlv_length tlvLength(DataStruct.getNameSize(), DataStruct.getContentSize());
    tlv_type tlvType(2);

    unsigned char buffer[sizeof(tlv_type) + sizeof(tlv_length) + DataStruct.getSize()];

    memcpy(buffer, &tlvType, sizeof(tlv_type));
    memcpy(buffer + sizeof(tlv_type), &tlvLength, sizeof(tlv_length));
    memcpy(buffer + sizeof(tlv_type) + sizeof(tlv_length), DataStruct.getByte(), DataStruct.getSize());
    
    getContainer()->getLinkLayer()->sendData(serverFd, buffer, sizeof(buffer));


}

char*
NdnLayer::combineCharAndInteger(char* c1, int num) {
    char str[10];
    sprintf(str, "%d", num);

    std::string strrrr;
    char* combine = new char[strlen(c1) + strlen(str) + 1];
    
    int i=0;
    for(i=0; i<strlen(c1); i++)
        combine[i] = c1[i];
    combine[i] = '/'; i++;
    for(i=0; i<strlen(str); i++)
        combine[strlen(c1) + 1 + i] = str[i];

    return combine;
}
char*
NdnLayer::combineChars(char* c1, char* c2) {
    char* combine = new char[strlen(c1) + strlen(c2) + 1];

    int i=0;
    for(i=0; i<strlen(c1); i++)
        combine[i] = c1[i];
    combine[i] = '/'; i++;

    for(i=0;i<strlen(c2); i++)
        combine[strlen(c1) + 1 + i] = c2[i];

    std::cout << "combine : " << combine << std::endl;
    std::cout << "combine size : " << strlen(combine) << std::endl;

    return combine;
}
