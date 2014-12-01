#include "Name.hpp"

#include <string.h>
#include <vector>

#include <iostream>

Name::Name() {
    name = NULL;
}

void
Name::setName(char* _name) {
/*    
    name.clear();

    std::cout << "Init Name size : " << sizeof(name) << std::endl;

    int length = strlen(_name);

    for(int i=0; i<length; i++) {
        name.push_back(_name[i]);
    }

    std::cout << "Name size : " <<  sizeof(name) << std::endl;
    */
    if(name != NULL) {
        std::cout << "name is not null" << std::endl;
        delete name;
    }

    int length = strlen(_name);
    name = new char[length + 1];

    strcpy(name, _name);
    name[length] = '\0';
}

char*
Name::getName() {
    
    /*
    int length = name.size();

    char* _name = new char[length];

    for(int i=0; i<length; i++) {
        _name[i] = name.at(i);
    }

    return _name;
    */

    return name;
}


void
Name::setSize(uint16_t len) {
    if( name != NULL ) {
        delete name;
    }
    name = new char[len];
}

uint16_t
Name::getSize() {
    return strlen(name);
}
