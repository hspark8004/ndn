#ifndef FRAGMENT_HPP_
    #define FRAGMENT_HPP_

#include <iostream>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "NdnlpData.hpp"
#include "DefineVariable.hpp"

class Fragment {
private : 
    size_t size;
    bool* fragmentArr;
    unsigned char* data;

public : 
    Fragment();
    Fragment(int fragCount, size_t totalSize);
    ~Fragment();
    unsigned char* getData();

    size_t getSize();
    bool assemble(NdnlpData& lp, unsigned char* buf);

    bool isCompleteAssemble();
};

#endif
