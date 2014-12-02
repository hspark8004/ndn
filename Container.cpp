#include <unordered_map>
#include "Container.hpp"
#include "Common.hpp"
#include "face/TcpSenderFace.hpp"
#include "face/TcpReceiverFace.hpp"

using namespace std;

Container::Container(char* com)
{
  comName = new char[strlen(com)];
  strcpy(comName, com);

  ndnLayer = new NdnLayer(this);
  linkLayer = new LinkLayer(this);
  ethernetLayer = new EthernetLayer(this);

  ClientConnectionMap = new unordered_map<int, Face*>;
  ServerConnectionMap = new unordered_map<int, Face*>;

  // FaceMap* c1 = new FaceMap(TCP_FACE);
  // FaceMap* c2 = new FaceMap(UDP_FACE);

  // ClientConnectionMap->insert({10, c1});
  // ClientConnectionMap->insert({20, c2});
  ClientConnectionMap->insert({10, new TcpSenderFace("/ndn", 10000)});

  // FaceMap* s1 = new FaceMap(TCP_FACE);
  // FaceMap* s2 = new FaceMap(UDP_FACE);

  // ServerConnectionMap->insert({123, s1});
  // ServerConnectionMap->insert({456, s2});
  ServerConnectionMap->insert({123, new TcpReceiverFace("/ndn", 20000)});
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

unordered_map<int, Face*>*
Container::getClientConnectionMap() {
    return ClientConnectionMap;
}

unordered_map<int, Face*>*
Container::getServerConnectionMap() {
    return ServerConnectionMap;
}

char*
Container::getComName() {
    return comName;
}
