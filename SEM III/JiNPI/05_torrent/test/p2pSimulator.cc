// JiNP 1.6 p2pSimulator - Wojciech Żółtak : 292583
#include <set>
#include <map>
#include <utility>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include "p2pSimulator.h"
using namespace std;
using namespace boost;

void Peer::addIncoming(Peer *peer)
{
  incoming.insert(peer);
  return ;
}

void Peer::removeIncoming(Peer *peer)
{
  incoming.erase(peer);
  return ;
}

void Peer::wakeup()
{
  set<Peer*>::iterator it;
  for(it = outcoming.begin(); it != outcoming.end(); it++)
   (*it)->accept(*this);
  return ;
}

void Peer::addPeer(Peer *neighbour)
{
  outcoming.insert(neighbour);
  neighbour->addIncoming(this);
  return ;
};

void Peer::removePeer(Peer *neighbour)
{
  outcoming.erase(neighbour);
  neighbour->removeIncoming(this);
}

Peer::~Peer()
// służy do zachowania spójności sieci przy zniszczeniu peera
{
  set<Peer*>::iterator it;
  for(it = outcoming.begin(); it != outcoming.end(); it++)
    removePeer((*it));
  // tak śmiesznie, bo removePeer unieważnia iterator,
  // ale `begin()` szczęśliwie jest w O(1)
  while((it = incoming.begin()) != incoming.end())
    (*it)->removePeer(this);
}


const Topology::iterator Topology::begin()
{ return peers.begin(); }
    
const Topology::iterator Topology::end()
{ return peers.end(); }
    
Peer* Topology::getPeer(iterator &it)
{ return &(*it->second); }
    
void Topology::addPeer(unsigned int id, Peer *peer)
{
  iterator it = peers.find(id);
  if(it != peers.end()) peers.erase(it);
  peers.insert(make_pair(id, shared_ptr<Peer>(peer)));
  return ;
}

void Topology::removePeer(unsigned int id)
{
  peers.erase(id);
  return ;
}
    
void Topology::addPeerConnection(unsigned int id1, unsigned int id2)
{
  iterator it1, it2, it3;
  it1 = peers.find(id1);
  it2 = peers.find(id2);
  if(it1 != peers.end() && it2 != peers.end())
    it1->second->addPeer(&(*it2->second));
  return ;
}

void Topology::removePeerConnection(unsigned int id1, unsigned int id2)
{
  iterator it1, it2;
  it1 = peers.find(id1);
  it2 = peers.find(id2);
  if(it1 != peers.end() && it2 != peers.end())
    it1->second->removePeer(&(*it2->second));
  return ;
}

void simulate(Topology &t, WakeupSequence &ws)
{
  Topology::iterator it;
  for(it = ws.begin(); it != ws.end(); it = ws.nextPeer())
    (t.getPeer(it))->wakeup();
  return ;
};

Topology* createRingTopology(PeerFactory &peerFactory, unsigned int n)
{
  unsigned int i;
  Topology *t = new Topology();
  for(i = 0; i < n; i++)
    t->addPeer(i, peerFactory.constructPeer(i));
  for(i = 0; i < n; i++)
    t->addPeerConnection(i, (i + 1) % n);
  return t;
};


void AvgCalcPeer::accept(Peer &peer)
{
  double newValue;
  // jeśli [peer] nie jest klasy `AvgCalPeer` rzuca `std::bad_cast`
  AvgCalcPeer &p = dynamic_cast<AvgCalcPeer&>(peer);
  newValue = (p.value + value) / 2;
  p.value = newValue;
  value = newValue;
  return ;
}
    
double AvgCalcPeer::getValue()
{ return value; }

Peer* AvgCalcPeerSinglePeakFactory::constructPeer(unsigned int id) const
{ return new AvgCalcPeer(id == 0 ? 1 : 0); }

const Topology::iterator LinearWakeupSequence::nextPeer()
{ return (c != e ? ++c : e); }
    
const Topology::iterator LinearWakeupSequence::begin() const
{ return b; }
    
const Topology::iterator LinearWakeupSequence::end() const
{ return e; }
