#ifndef DATA_HPP_
    #define DATA_HPP_

#include <iostream>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "tlv_type.hpp"
#include "tlv_length.hpp"
#include "MetaInfo.hpp"

using namespace std;

class Data {
    private : 
        char* m_name;
        MetaInfo meta;
//        unsigned char* content;
        size_t contentSize;
        unsigned char* content;
        uint8_t signature;

    public : 
        Data();
        Data(char* _name, unsigned char* _data, uint64_t size);
        void setName(char* _name);
        void setName(char* _name, uint16_t name_length);
        void setMetaInfo(MetaInfo m);
        void setContent(unsigned char* content, uint64_t size);
        void setSignature(uint8_t sig);

        void setNameSize(size_t len);
        void setContentSize(size_t len);

        void setData(unsigned char* packet, tlv_length length);
        char* getName();
        MetaInfo getMeta();
        unsigned char* getContent();
        uint8_t getSignature();

        uint16_t getNameSize();
        uint64_t getContentSize();

        uint64_t getSize();
        unsigned char* getByte();

        void initializingData(tlv_length tlv);
        
        void showData();


        // data name
        // ex) org/secmem/gn/a/1~4
        //char* extractComName(); // org/secmem/gn
        //char* extractAppName(); // a
        //char* extractDataNum(); // 1~4

        // ex) org/secmem/gn/a
        char* extractComName();
        char* extractAppName();

        void appendName(int number);
};

#endif
