#ifndef FACEMAP_HPP_
    #define FACEMAP_HPP_

#include "DefineVariable.hpp"

class FaceMap {
    private : 
        int ConnectType;

    public : 
        FaceMap(int type);
        void setConnectType(int type);
        int getConnectType();
};


#endif
