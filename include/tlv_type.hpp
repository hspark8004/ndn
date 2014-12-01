#ifndef TLV_TYPE_HPP_
    #define TLV_TYPE_HPP_

#include <stdint.h>
#include <unistd.h>

class tlv_type {
    
private : 
    uint8_t type;
    // type
    // 1 : interest
    // 2 : data

public : 
    tlv_type() {}

    tlv_type(uint8_t _type) {
        type = _type;
    }

    void setTlvType(uint8_t _type) {
        type = _type;
    }
    uint8_t getTlvType() {
        return type;
    }
};

#endif
