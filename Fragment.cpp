#include "Fragment.hpp"

Fragment::Fragment() {

}

Fragment::Fragment(int fragCount, size_t totalSize) {
    data = new unsigned char[totalSize];
    size = totalSize;

    fragmentArr = new bool[fragCount];
    for(int i=0; i<fragCount; i++)
        fragmentArr[i] = false;
}

Fragment::~Fragment() {
    std::cout << "Fragment::~Fragment()" << std::endl;

    delete fragmentArr;
    delete data;
}
unsigned char*
Fragment::getData() {
    return data;
}

size_t
Fragment::getSize() {
    return size;
}

bool
Fragment::assemble(NdnlpData& lp, unsigned char* buf) {
    std::cout << "void Fragment::assemble(NdnlpData& lp, unsigned char* data)" << std::endl;

    if(fragmentArr[lp.getFragIndex()] == true) {
        std::cout << "duplicated particle!" << std::endl;
        return false;
    }
    for(int i=0; i<lp.getFragCount(); i++) {
        if(fragmentArr[i])
            std::cout << "True" << std::endl;
        else
            std::cout << "False" << std::endl;
    }

    for(int i=0; i<lp.getPayload(); i++)
        data[lp.getFragIndex() * PAYLOAD_SIZE + i] = buf[i];

    fragmentArr[lp.getFragIndex()] = true;

    return isCompleteAssemble();
}

bool
Fragment::isCompleteAssemble() {
    bool check = true;

    int arrCount = size / PAYLOAD_SIZE;
    
    for(int i=0; i<arrCount; i++) {
        if( fragmentArr[i] == false ) {
            check = false;
            break;
        }
    }
    if(!check)
        return false;

    return true;
}
