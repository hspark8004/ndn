#ifndef CONTAINER_HPP_
#define CONTAINER_HPP_

#include "NdnLayer.hpp"
#include "LinkLayer.hpp"
#include "EthernetLayer.hpp"
#include "ReqInformation.hpp"
#include "face/Face.hpp"

#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

class NdnLayer;
class LinkLayer;
class EthernetLayer;
class Container
{
private : 
  char* comName;

  NdnLayer* ndnLayer;
  LinkLayer* linkLayer;
  EthernetLayer* ethernetLayer;

  unordered_map<int, Face*>* ClientConnectionMap;
  unordered_map<int, Face*>* ServerConnectionMap;
public : 
  Container(char* com);

  NdnLayer* getNdnLayer();
  LinkLayer* getLinkLayer();
  EthernetLayer* getEthernetLayer();

  unordered_map<int, Face*>* getClientConnectionMap();
  unordered_map<int, Face*>* getServerConnectionMap();

  char* getComName();
};
#endif
