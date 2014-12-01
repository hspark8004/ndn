#ifndef TLV_LENGTH_HPP_
    #define TLV_LENGTH_HPP_

#include <iostream>
#include <stdint.h>
#include <unistd.h>

class tlv_length {
    private : 
        uint16_t length_name;
        uint16_t length_data;

    public : 
        tlv_length() {}

        tlv_length(uint16_t len_name, uint16_t len_data) {
            length_name = len_name;
            length_data = len_data;
        }

        void setNameLength(uint16_t len) {
            length_name = len;
        }
        void setDataLength(uint16_t len) {
            length_data = len;
        }
        uint16_t getNameLength() {
            return length_name;
        }
        uint16_t getDataLength() {
            return length_data;
        }

        void showTlvLength() {
            std::cout << "===== tlv length =====" << std::endl;
            std::cout << "name length : " << getNameLength() << std::endl;
            std::cout << "data length : " << getDataLength() << std::endl;
            std::cout << "======================" << std::endl;
        }
};

#endif
