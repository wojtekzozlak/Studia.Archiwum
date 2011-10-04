// JiNP 1.6 p2pSimulator - Wojciech Żółtak : 292583
#include <set>
#include <map>
#include <utility>
#include <iostream>
#include <boost/shared_ptr.hpp>
using namespace std;
using namespace boost;

class Peer
{
  private:
    set<Peer*> outcoming;
    set<Peer*> incoming;
    void addIncoming(Peer *peer);
    void removeIncoming(Peer *peer);
    
  public:
    void wakeup();  
    virtual void accept(Peer &peer) = 0;
    void addPeer(Peer *neighbour);
    void removePeer(Peer *neighbour);
    virtual ~Peer();
};

class PeerFactory
{
  public:
    virtual Peer* constructPeer(unsigned int id) const = 0;
    virtual ~PeerFactory() {};
};

class Topology
{
  private:
    typedef map< unsigned int, shared_ptr<Peer> > PeersMap;
    PeersMap peers;

  public:
    typedef PeersMap::iterator iterator;
    
    const iterator begin();
    const iterator end();
    Peer* getPeer(iterator &it);
    void addPeer(unsigned int id, Peer *peer);
    void removePeer(unsigned int id);
    void addPeerConnection(unsigned int id1, unsigned int id2);
    void removePeerConnection(unsigned int id1, unsigned int id2);
};

class WakeupSequence
{
  public:
    // to nie jest normalny iterator, więc nie chcemy, żeby go zmieniać
    virtual const Topology::iterator nextPeer() = 0;
    virtual const Topology::iterator begin() const = 0;
    virtual const Topology::iterator end() const = 0;
    virtual ~WakeupSequence() {};
};

void simulate(Topology &t, WakeupSequence &ws);

Topology* createRingTopology(PeerFactory &peerFactory, unsigned int n);

class AvgCalcPeer : public Peer
{
  private:
    double value;
  
  public:  
    AvgCalcPeer(double v) : value(v) {};
    double getValue();
    virtual ~AvgCalcPeer() {};
    
    virtual void accept(Peer &peer);
    // może rzucić `std::bad_cast` jeśli [peer] nie jest klasy `AvgCalcPeer`
};

class AvgCalcPeerSinglePeakFactory : public PeerFactory
{
  public:
    virtual Peer* constructPeer(unsigned int id) const;
};

class LinearWakeupSequence : public WakeupSequence
{
  private:
    Topology::iterator b, e, c;

  public:
    LinearWakeupSequence(Topology::iterator begin, Topology::iterator end) :
        b(begin), c(begin), e(end) {};

    // to nie jest normalny iterator, więc nie chcemy, żeby go zmieniać
    const Topology::iterator nextPeer();
    const Topology::iterator begin() const;
    const Topology::iterator end() const;
};
