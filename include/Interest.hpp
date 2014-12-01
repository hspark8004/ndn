#ifndef INTEREST_HPP_
    #define INTEREST_HPP_

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include "tlv_type.hpp"
#include "tlv_length.hpp"

class Interest {
private :
    char* m_name;
    uint8_t m_selector;
    uint8_t m_scope;
    uint64_t m_interestLifetime;

public :

    Interest();
    Interest(char* _name, uint8_t _selector, uint8_t _scope, uint64_t _time);
    
    //    setName(_name);
    //    setSelector(_selector);
    //    setScope(_scope);
    //    setTime(_time);
    //}
    ~Interest();
    void setName(char* _name);
    char* getName();

    // interest name
    // ex) org/secmem/gn/a/a.mp4
    // extractComName() : org/secmem/gn
    // extractAppName() : a
    // extractReqName() : a.mp4

    char* extractComName();
    int extractFileDescriptor();
    char* extractReqName();    // extract

    void setNameSize(uint16_t len);
    uint16_t getNameSize();


    void setSelector(uint8_t _selector);
    uint8_t getSelector();

    void setScope(uint8_t _scope);
    uint8_t getScope();

    void setTime(uint64_t _time);
    uint64_t getTime();

    uint64_t size();

    unsigned char* getByte();

    void InitializingInterest(tlv_length tlv);
    void setInterest(unsigned char* data, uint16_t name_length);

    void showInterestData();
};

#endif
