#include "FaceMap.hpp"

FaceMap::FaceMap(int type) {
    ConnectType = type;
}

void
FaceMap::setConnectType(int type) {
    ConnectType = type;
}

int
FaceMap::getType() {
    return ConnectType;
}
