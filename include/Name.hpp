#include <iostream>
#include <vector>
#include <string>
#include <stdint.h>
#include <unistd.h>

class Name {
private : 
//    std::vector<char> name;
//    std::string s;
    char* name;

public : 
    Name();

    void setName(char* _name);
    char* getName();

    void setSize(uint16_t len);
    uint16_t getSize();
//    void setNameSize(int length);
//    unsigned long size();
};
