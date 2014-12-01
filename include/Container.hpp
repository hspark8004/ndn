#ifndef CONTAINER_HPP_
    #define CONTAINER_HPP_

#include "NdnLayer.hpp"
#include "LinkLayer.hpp"
#include "EthernetLayer.hpp"
#include "ReqInformation.hpp"

#include "FaceMap.hpp"

#include <iostream>
#include <vector>

#include <unordered_map>

using namespace std;

class NdnLayer;
class LinkLayer;
class EthernetLayer;

class Container {
private : 
    char* comName;

    NdnLayer* ndnLayer;
    LinkLayer* linkLayer;
    EthernetLayer* ethernetLayer;

    int NextRecvInterestsIndex;
    vector<ReqInformation>* RecvInterests;

    unordered_map<int, FaceMap*>* ClientConnectionMap;
    unordered_map<int, FaceMap*>* ServerConnectionMap;
public : 
    Container(char* com);

    NdnLayer* getNdnLayer();
    LinkLayer* getLinkLayer();
    EthernetLayer* getEthernetLayer();
    vector<ReqInformation>* getRecvInterests();

    unordered_map<int, FaceMap*>* getClientConnectionMap();
    unordered_map<int, FaceMap*>* getServerConnectionMap();

    char* getComName();
    void addInterestInformation(Interest interest, uint8_t* shost_mac);
    void showInterestInformation();
    ReqInformation* getInterestInformation(int serverFd);

};


#endif
