#include "LinkLayer.hpp"

#include <iostream>

//#define PAYLOAD_SIZE 1000

LinkLayer::LinkLayer() {
    std::cout << "LinkLayer()" << std::endl;
}

LinkLayer::LinkLayer(Container* container) {
    std::cout << "LinkLayer(Container* container)" << std::endl;
    this->container = container;
}

Container*
LinkLayer::getContainer() {
    return container;
}


int
LinkLayer::sendInterest(int fd, unsigned char* data, uint64_t size) {
    std::cout<< "sendInterest(char* data)" << std::endl;

    std::cout << "tlv + interest size : " << size << std::endl;

    if( size > PAYLOAD_SIZE ) {
        std::cout << "Interest + tlv > THRESHOLD_PAYLOAD_SIZE" << std::endl;
    } else {
        std::cout << "Interest + tlv < THRESHOLD_PAYLOAD_SIZE" << std::endl;
    }

    auto search = getContainer()->getClientConnectionMap()->find(fd);
    if(search == getContainer()->getClientConnectionMap()->end()) {
        std::cout << "등록되지 않은 fd" << std::endl;
        return -1;
    }

    int LpFlag = 0;
    switch(search->second->getType())
    {
        case TCP_FACE : LpFlag = 1; break;
        case UDP_FACE : LpFlag = 0; break;
        default : 
                        std::cout << "FaceMap::getConnectionType() 값이 이상함" << std::endl;
                        return -2;
    }

    int packetCount = size / PAYLOAD_SIZE;
    if( size % PAYLOAD_SIZE > 0 )
        packetCount++;

    uint64_t packetId = rand(); // create packet ID
    for(int i=0; i<packetCount; i++) {
        NdnlpData ndnlp(
                packetId + i,
                LpFlag,
                i,
                packetCount,
                packetCount-i > 1 ? PAYLOAD_SIZE : size % PAYLOAD_SIZE
                );
        unsigned char buf[sizeof(NdnlpData) + ndnlp.getPayload()];
        memcpy(buf, &ndnlp, sizeof(NdnlpData));
        memcpy(buf + sizeof(NdnlpData), data + PAYLOAD_SIZE * i, ndnlp.getPayload());

        getContainer()->getEthernetLayer()->sendInterest(buf, sizeof(NdnlpData) + size);
    }
    return 1;
}

int
LinkLayer::sendInterest(Interest interest) {
    std::cout << "sendInterest(Interest interest)" << std::endl;
}

int
LinkLayer::sendInterest(tlv_type type, tlv_length length, Interest interest) {
    std::cout << "sendInterst(tlv_type type, tlv_length length, Interest interest)" << std::endl;
}


int
LinkLayer::sendData(int serverFd, unsigned char* data, uint64_t size) {
    std::cout << "int LinkLayer::sendData(unsigned char* data, uint64_t size)" << std::endl;

    std::cout << "tlv + data size : " << size << std::endl;

    auto search = getContainer()->getServerConnectionMap()->find(serverFd);
    if(search == getContainer()->getServerConnectionMap()->end())
    {
        std::cout << "serverFd를 찾을 수 없습니다!" << std::endl;
        return -1;
    }
    int LpFlags;
    switch(search->second->getType())
    {
        case TCP_FACE : LpFlags = 1; break;
        case UDP_FACE : LpFlags = 0; break;
        default : std::cout << "Not Defined FaceMap Connect Type" << std::endl; break;
    }

    if( size > PAYLOAD_SIZE ) {
        std::cout << "tlv + Data > THRESHOLD_PAYLOSD_SIZE" << std::endl;
    } else {
        std::cout << "tlv + Data < THRESHOLD_PAYLOAD_SIZE" << std::endl;
    }
    int packetCount = size / PAYLOAD_SIZE;
    if( size % PAYLOAD_SIZE > 0 )
        packetCount++;

    uint64_t packetId = rand();
    for(int i=0; i<packetCount; i++) {
        NdnlpData ndnlp(
                packetId + i,   /*packet SeqNum*/
                LpFlags,              /*packet Flags*/                    
                i,              /*FragIndex*/
                packetCount,     /*FragCount*/
                packetCount-i > 1 ? PAYLOAD_SIZE : size % PAYLOAD_SIZE
                );
        unsigned char buf[sizeof(NdnlpData) + ndnlp.getPayload()];
        memcpy(buf, 
                &ndnlp,
                sizeof(NdnlpData));

        memcpy(buf + sizeof(NdnlpData), 
                data + PAYLOAD_SIZE * i ,
                    ndnlp.getPayload());

        getContainer()->getEthernetLayer()->sendData(serverFd, buf, sizeof(buf));

        // temporary save the packet to sent packet store! (key : lp.SeqNum, data : packet byte array);
    }
}


void
LinkLayer::recvNdnPacket(unsigned char* packet, uint8_t* shost_mac) {
    std::cout << "void LinkLayer::recvNdnPacket(unsigned char* packet)" << std::endl;

    NdnlpData lp;
    memcpy(&lp, packet, sizeof(NdnlpData));

    lp.showNdnlpData();


    if(lp.getFlags() == 0)  // UDP
        getContainer()->getNdnLayer()->recvNdnPacket(packet + sizeof(NdnlpData), shost_mac);
    else if(lp.getFlags() == 1) {   // TCP
        // Add SeqNum to Ack Queue!
        std::cout << "NDN_TCP Receive! please add SeqNum to Ack Queue!" << std::endl;

        if(lp.getFragCount() == 1)
            getContainer()->getNdnLayer()->recvNdnPacket(packet + sizeof(NdnlpData), shost_mac);
        else {
            // 분할패킷 등록 또는 조립
            constructTempStore(lp, packet + sizeof(NdnlpData), shost_mac);
        }
    }
    /*

    if(lp.getFragCount() == 1) {
        getContainer()->getNdnLayer()->recvNdnPacket(packet + sizeof(NdnlpData), shost_mac);
    } else {
        // 분할패킷 등록 또는 조립.
        constructTempStore(lp, packet + sizeof(NdnlpData), shost_mac);
    }*/
}

void
LinkLayer::constructTempStore(NdnlpData& lp, unsigned char* data, uint8_t* shost_mac) {
    std::cout << "void LinkLayer::constructTempStore(NdnlpData& lp, unsigned char* data)" << std::endl;

    // step 1) find
    std::cout << "lp Seq : " << lp.getSeqNum();
    std::cout << ", lp.FragIdx : " << lp.getFragIndex() << std::endl;


    auto search = temporaryStore.find(lp.getSeqNum() - lp.getFragIndex());
    
    if(search == temporaryStore.end()) {
        std::cout << "not found!" << std::endl;
        // step 2) add
        registerTempStore(lp, data, shost_mac);
    }
    else {
        // step 3) assembly
        std::cout << "found! assembly!" << std::endl;
        assemblyTempStore(lp, data, shost_mac); 
    }

    std::cout << "temporaryStore contains" << std::endl;

    for(auto iter = temporaryStore.begin(); iter != temporaryStore.end(); ++iter) {
        std::cout << iter->first << std::endl;
    }

    std::cout << "temporaryStore end" << std::endl;
}

void
LinkLayer::registerTempStore(NdnlpData& lp, unsigned char* data, uint8_t* shost_mac) {
    std::cout << "void LinkLayer::registerTempStore(NdnlpData& lp, unsigned char* data)" << std::endl;

    Fragment* frag = new Fragment(lp.getFragCount(), lp.getFragCount() * PAYLOAD_SIZE);

    temporaryStore.insert({ lp.getSeqNum()-lp.getFragIndex(), frag});

    assemblyTempStore(lp, data, shost_mac);

}

void
LinkLayer::assemblyTempStore(NdnlpData& lp, unsigned char* data, uint8_t* shost_mac) {
    std::cout << "void LinkLayer::assemblyTempStore(NdnlpData& lp, unsigned char* data)" << std::endl;

    auto search = temporaryStore.find(lp.getSeqNum() - lp.getFragIndex());

    if(search == temporaryStore.end())
        std::cout << "What the Fuck?" << std::endl;
    else {
        std::cout << search->first << std::endl;
        
        bool resultAssemble = search->second->assemble(lp, data);
       
        if(!resultAssemble)
            return;
        
        std::cout << "???" << std::endl;
        getContainer()->getNdnLayer()->recvNdnPacket(search->second->getData(), shost_mac);

        delete search->second;
        temporaryStore.erase(lp.getSeqNum() - lp.getFragIndex());        
    }
}
