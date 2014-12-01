#include "Data.hpp"
#include <string.h>

Data::Data() {
}

Data::Data(char* _name, unsigned char* _data, uint64_t size) {
    setName(_name);
    setContent(_data, size);
    setSignature(0);
    MetaInfo m(1,2,3);
    setMetaInfo(m);
}

Data::~Data() {
    std::cout << "Data::~Data()" << std::endl;
    delete m_name;
    delete content;
}

void
Data::setName(char* _name) {
    m_name = new char[strlen(_name)];
    strcpy(m_name, _name);
}

void
Data::setName(char* _name, uint16_t name_length) {
    m_name = new char[name_length];

    for(int i=0; i<name_length; i++)
        m_name[i] = _name[i];
}
void
Data::setMetaInfo(MetaInfo m) {
    meta.setContentType(m.getContentType());
    meta.setFreshnessPeriod(m.getFreshnessPeriod());
    meta.setFinalBlockId(m.getFinalBlockId());
}

void
Data::setContent(unsigned char* _content, uint64_t size) {
    content = new unsigned char[size];

    for(int i=0; i<size; i++)
        content[i] = _content[i];

    contentSize = size;
}

void
Data::setSignature(uint8_t sig) {
    signature = sig;
}

char*
Data::getName() {
    return m_name;
}

MetaInfo
Data::getMeta() {
    return meta;
}

unsigned char* 
Data::getContent() {
    return content;
}

uint8_t
Data::getSignature() {
    return signature;
}

uint16_t 
Data::getNameSize() {
    return strlen(m_name);
}

uint64_t
Data::getContentSize() {
    return contentSize;
}

uint64_t
Data::getSize() {
    return
        getNameSize() + 
        sizeof(MetaInfo) +
        sizeof(size_t) +
        getContentSize() +
//        sizeof(size_t) + 
        sizeof(signature);
}

unsigned char*
Data::getByte() {
    unsigned char buffer[getSize()];

    // m_name
    memcpy(buffer, m_name, getNameSize());
    // meta
    memcpy(buffer + getNameSize(), &meta, sizeof(MetaInfo));
    // contentSize
    memcpy(buffer + getNameSize() + sizeof(MetaInfo), &contentSize, sizeof(contentSize));
    // content
    memcpy(buffer + getNameSize() + sizeof(MetaInfo) + sizeof(contentSize), content, getContentSize());
    // signature
    memcpy(buffer + getNameSize() + sizeof(MetaInfo) + sizeof(contentSize) + getContentSize(), &signature, sizeof(signature));

    return buffer;
}

void
Data::showData() {
    cout << "===== Data Class =====" << endl;
    cout << "Name : " << m_name << endl;
    cout << "MetaInfo : 1) ContentType : " << (unsigned int)meta.getContentType() << endl;
    cout << "MetaInfo : 2) FreshnessPeriod : " << meta.getFreshnessPeriod() << endl;
    cout << "MetaInfo : 3) FinalBlockId : " << meta.getFinalBlockId() << endl;
    cout << "Content Size " << getContentSize() << endl;
    cout << "Content : " << content << endl;
    cout << "Signature : " << (unsigned int)signature << endl;
    cout << "======================" << endl;
}

/*
char*
Data::extractComName() {
    int size = getNameSize();

    int cutIdx = -1;
    int checkCount = 0;

    for(int i=size-1; i>=0; i--) {
        char c = getName()[i];
        if(c=='/') {
            cutIdx = i;
            checkCount++;
        }
        if(checkCount==2)
            break;
    }
    char* comName = new char[cutIdx];

    for(int i=0; i<cutIdx; i++)
        comName[i] = getName()[i];

    return comName;
}

char*
Data::extractAppName() {
    int size = getNameSize();

    int cutStartIdx = -1;
    int cutEndIdx = -1;

    int checkCount = 0;
    for(int i=size-1; i>=0; i--) {
        char c = getName()[i];
        if(c=='/') {
            switch(checkCount) {
                case 0 : cutEndIdx = i; break;
                case 1 : cutStartIdx = i; break;
            }
            checkCount++;
        }
        if(checkCount==2)
            break;
    }
    int arrSize = cutEndIdx - cutStartIdx - 1;
    char* appName = new char[arrSize];

    for(int i=0; i<arrSize; i++)
        appName[i] = getName()[cutStartIdx + 1 + i];

    return appName;
}

char*
Data::extractDataNum() {
    int size = getNameSize();

    int cutIdx = -1;
    for(int i=size-1; i>=0; i--) {
        char c = getName()[i];
        if(c=='/') {
            cutIdx = i;
            break;
        }
    }
    if(cutIdx == -1) {
        std::cout << "extractData error!" << std::endl;
    }
    
    int arrSize = getNameSize() - cutIdx - 1;
    
    char* dataNum = new char[arrSize];

    for(int i=0; i<arrSize; i++)
        dataNum[i] = getName()[cutIdx + 1 + i];

    return dataNum;
}
*/

char*
Data::extractComName() {
    int size = getNameSize();

    int cutIdx = -1;
    for(int i=size-1; i>=0; i--){
        char c = getName()[i];

        if(c=='/') {
            cutIdx = i;
            break;
        }
    }
    char* comName = new char[cutIdx];

    for(int i=0; i<cutIdx; i++)
        comName[i] = getName()[i];

    return comName;
}

char*
Data::extractAppName() {
    int size = getNameSize();

    int cutIdx = -1;
    for(int i=size-1; i>=0; i--) {
        char c = getName()[i];
        if(c=='/') {
            cutIdx = i;
            break;
        }
    }
    int arrSize = getNameSize() - cutIdx - 1;
    char* appName = new char[arrSize];

    for(int i=0; i<arrSize; i++)
        appName[i] = getName()[cutIdx + 1 + i];

    return appName;
}

void
Data::appendName(int number) {
    char numBuf[10];
    sprintf(numBuf, "%d", number);

    char* newName = new char[getNameSize() + strlen(numBuf) + 1];

    // original name copy
    for(int i=0; i<getNameSize(); i++)
        newName[i] = getName()[i];

    // '/' copy
    newName[getNameSize()] = '/';

    // number copy
    for(int i=0; i<sizeof(numBuf); i++) {
        newName[getNameSize() + 1 + i] = numBuf[i];
    }
    
    // initializing name
    delete m_name;

    setName(newName);
}

void
Data::setNameSize(size_t len) {
    std::cout << "void Data::setNameSize(size_t len)" << std::endl;
    std::cout << "len : " << len << std::endl;
    m_name = new char[len];
}

void
Data::setContentSize(size_t len) {
    std::cout << "void Data::setContentsize(size_t len)" << std::endl;
    std::cout << "len : " << len << std::endl;

    content = new unsigned char[len];
    contentSize = len;
}

void
Data::setData(unsigned char* packet, tlv_length length) {
    std::cout << "void Data::setData(unsigned char* packet, tlv_length length)" << std::endl;

    // set Name
    setName((char*)packet, length.getNameLength());
    

    // setMetaInfo;
    memcpy(&meta, packet + length.getNameLength(), sizeof(MetaInfo));

    // setContentSize;
    memcpy(&contentSize, packet + length.getNameLength() + sizeof(MetaInfo),
            sizeof(size_t));

    // setContent
    setContent(packet + 
                length.getNameLength() + 
                sizeof(MetaInfo) + 
                sizeof(size_t),
                length.getDataLength());

    // setSignature
    memcpy(&signature, 
            packet + 
            length.getNameLength() + 
            sizeof(MetaInfo) + 
            sizeof(size_t) + 
            length.getDataLength(),
            sizeof(signature));
}
