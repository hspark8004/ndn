#include "Container.hpp"

Container::Container(char* com) {

    NextRecvInterestsIndex = 1;

    comName = new char[strlen(com)];
    strcpy(comName, com);

    ndnLayer = new NdnLayer(this);
    linkLayer = new LinkLayer(this);
    ethernetLayer = new EthernetLayer(this);

    RecvInterests = new vector<ReqInformation>();

    ClientConnectionMap = new unordered_map<int, FaceMap*>;
    ServerConnectionMap = new unordered_map<int, FaceMap*>;

    FaceMap* c1 = new FaceMap(TCP_FACE);
    FaceMap* c2 = new FaceMap(UDP_FACE);

    ClientConnectionMap->insert({10, c1});
    ClientConnectionMap->insert({20, c2});

    FaceMap* s1 = new FaceMap(TCP_FACE);
    FaceMap* s2 = new FaceMap(UDP_FACE);

    ServerConnectionMap->insert({123, s1});
    ServerConnectionMap->insert({456, s2});
}

NdnLayer*
Container::getNdnLayer() {
    return ndnLayer;
}

LinkLayer*
Container::getLinkLayer() {
    return linkLayer;
}

EthernetLayer*
Container::getEthernetLayer() {
    return ethernetLayer;
}

unordered_map<int, FaceMap*>*
Container::getClientConnectionMap() {
    return ClientConnectionMap;
}

unordered_map<int, FaceMap*>*
Container::getServerConnectionMap() {
    return ServerConnectionMap;
}

vector<ReqInformation>*
Container::getRecvInterests() {
    return RecvInterests;
}

char*
Container::getComName() {
    return comName;
}

void
Container::addInterestInformation(Interest interest, uint8_t* shost_mac) {
    ReqInformation req(NextRecvInterestsIndex, interest, shost_mac);
    RecvInterests->push_back(req);

    NextRecvInterestsIndex++;
}

void
Container::showInterestInformation() {
    for(int i=0; i<RecvInterests->size(); i++)
    {
        RecvInterests->at(i).showInformation();
    }
}

ReqInformation*
Container::getInterestInformation(int serverFd) {
    for(int i=0; i<RecvInterests->size(); i++)
    {
        if( serverFd == RecvInterests->at(i).getServerFd() )
            return &RecvInterests->at(i);
    }
    return NULL;
}
