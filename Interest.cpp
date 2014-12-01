#include "Interest.hpp"

Interest::Interest() {

}

Interest::Interest(char* _name, uint8_t _selector, uint8_t _scope, uint64_t _time)
{
    setName(_name);
    setSelector(_selector);
    setScope(_scope);
    setTime(_time);
}
Interest::~Interest() {
    std::cout << "Interest::~Interest()" << std::endl;
    //delete m_name;
}
void
Interest::setName(char* _name)
{
    std::cout << "void Interest::setName(char* _name)" << std::endl;
    
    int length = strlen(_name);
    
    m_name = new char[length];

    strcpy(m_name, _name);
}

// the name of Interest means computer name and request data name
char*
Interest::getName() {
    return m_name;
}

char*
Interest::extractComName() {
    int size = getNameSize();

    int cutIdx = -1;

    int checkCount = 0;
    for(int i=size-1; i>=0; i--) {
        char c = getName()[i];
        if(c == '/') {
            cutIdx = i;
            checkCount++;
        }
        if(checkCount == 2)
            break;
    }
    char* comName = new char[cutIdx];
    
    for(int i=0; i<cutIdx; i++) {
        comName[i] = getName()[i];
    }
    return comName;
}

int
Interest::extractFileDescriptor() {
    int size = getNameSize();

    int cutStartIdx = -1;
    int cutEndIdx = -1;

    int checkCount = 0;
    for(int i=size-1; i>=0; i--) {
        char c = getName()[i];
        if(c == '/') {
            switch(checkCount) {
                case 0 : cutEndIdx = i; break;
                case 1 : cutStartIdx = i; break;
            }
            checkCount++;
        }
        if(checkCount == 2)
            break;
    }
    int arrSize = cutEndIdx - cutStartIdx - 1;
    char* appName = new char[arrSize];

    //memcpy(appName, getName() + cutStartIdx + 1, arrSize);
    for(int i=0; i<arrSize; i++)
        appName[i] = getName()[cutStartIdx + 1 + i];
    
    
    //return appName;
    return atoi(appName);
}

char*
Interest::extractReqName() {
    int size = getNameSize();

    int cutIdx = -1;
    for(int i=size-1; i>=0; i--)
    {
        char c = getName()[i];
        if(c == '/') {
            cutIdx = i;
            break;
        }
    }
    if(cutIdx == -1) {
        std::cout << "extractData error!" << std::endl;
    }
    
    int arrSize = getNameSize() - cutIdx - 1;
    char* reqName = new char[arrSize];

    for(int i=0; i<arrSize; i++)
        reqName[i] = getName()[cutIdx+1+i];

    return reqName;
}

void
Interest::setSelector(uint8_t _selector)
{
    m_selector = _selector;

}

uint8_t
Interest::getSelector() {
    return m_selector;
}

void
Interest::setScope(uint8_t _scope)
{
    m_scope = _scope;
}

uint8_t
Interest::getScope() {
    return m_scope;
}

void
Interest::setTime(uint64_t _time)
{
    m_interestLifetime = _time;
}

uint64_t
Interest::getTime() {
    return m_interestLifetime;
}

uint64_t
Interest::size() {
    return getNameSize() + 
        sizeof(m_selector) + 
        sizeof(m_scope) +
        sizeof(m_interestLifetime);
}


uint16_t
Interest::getNameSize() {
    return strlen(m_name);
}

void
Interest::setNameSize(uint16_t len) {
    std::cout << "len : " << len << std::endl;
    m_name = new char[len];
}

unsigned char*
Interest::getByte() {
    unsigned char data[size()];

    memcpy(data, m_name, getNameSize());


    

    memcpy(data + getNameSize(), &m_selector, sizeof(m_selector));
    memcpy(data + getNameSize() + sizeof(m_selector), &m_scope, sizeof(m_scope));
    memcpy(data + getNameSize() + sizeof(m_selector) + sizeof(m_scope), &m_interestLifetime, sizeof(m_interestLifetime));

    return data;
}

void
Interest::InitializingInterest(tlv_length tlv) {
    m_name = new char[tlv.getNameLength()];
}

void
Interest::setInterest(unsigned char* data, uint16_t name_length) {
    // name copy
    memcpy(m_name, data, name_length);

    // selector copy
    memcpy(&m_selector, data + name_length, sizeof(m_selector));

    // m_scope copy
    memcpy(&m_scope, data + name_length + sizeof(m_selector), sizeof(m_scope));

    // m_interestLifetime copy
    memcpy(&m_interestLifetime, data + name_length + sizeof(m_selector) + sizeof(m_scope), sizeof(m_interestLifetime));
}

void
Interest::showInterestData() {
    std::cout << "==== Interest Data =====" << std::endl;
    std::cout << "Name : " << getName() << std::endl;
    std::cout << "Selector : " << (unsigned int)getSelector() << std::endl;
    std::cout << "Scope : " << (unsigned int)getScope() << std::endl;
    std::cout << "Time : " << getTime() << std::endl;
    std::cout << "========================" << std::endl;
}
