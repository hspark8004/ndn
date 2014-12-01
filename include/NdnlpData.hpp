#ifndef NDNLPDATA_HPP_
    #define NDNLPDATA_HPP_

#include <iostream>
#include <unistd.h>
#include <stdint.h>

class NdnlpData {
private : 
    uint64_t SeqNum;
    uint16_t Flags;
    uint16_t FragIndex;
    uint16_t FragCount;
    size_t payload;

public : 
    NdnlpData() { }

    NdnlpData(uint64_t seq, uint16_t flag, uint16_t idx, uint16_t count, size_t size)
    {
        SeqNum = seq;
        Flags = flag;
        FragIndex = idx;
        FragCount = count;
        payload = size;
    }

    void setSeqNum(uint64_t seq) { SeqNum = seq; }
    void setFlags(uint16_t flag) { Flags = flag; }
    void setFlagIndex(uint16_t idx) { FragIndex = idx; }
    void setFragCount(uint16_t count) { FragCount = count; }
    void setPayload(size_t size) { payload = size; }

    uint64_t getSeqNum() { return SeqNum; }
    uint16_t getFlags() { return Flags; }
    uint16_t getFragIndex() { return FragIndex; }
    uint16_t getFragCount() { return FragCount; }
    size_t getPayload() { return payload; }

    void showNdnlpData() {
        std::cout << "===== NdnlpData =====" << std::endl;
        std::cout << "SeqNum : " << getSeqNum() << std::endl;
        std::cout << "Flags : " << getFlags() << std::endl;
        std::cout << "FragIndex : " << getFragIndex() << std::endl;
        std::cout << "FragCount : " << getFragCount() << std::endl;
        std::cout << "Payload : " << getPayload() << std::endl;
        std::cout << "=====================" << std::endl;
    }
};

#endif
