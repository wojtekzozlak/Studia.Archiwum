#include "p2pSimulator.h"
#include <cassert>
#include <vector>
#include <iostream>

using namespace std;

unsigned int const maxPeers = 10;

class TestPeer: public Peer
{
public:
	TestPeer():
		visited(false)
	{
		++TestPeer::objectsInLive;
	}

	TestPeer(TestPeer const &)
	{
		++TestPeer::objectsInLive;
	}

	TestPeer & operator=(TestPeer const&)
	{
		++TestPeer::objectsInLive;
		return *this;
	}

	~TestPeer() {--TestPeer::objectsInLive;}	

    virtual void accept(Peer &)
	{
		assert(!this->visited);
		this->visited = true;
	}

	void reset()
	{
		this->visited = false;
	}

	bool wasVisited() const
	{
		return this->visited;
	}

	static int objectsInLive;
private:
	bool visited;
};

int TestPeer::objectsInLive = 0;

class TestPeerFactory: public PeerFactory
{
public:
    virtual Peer * constructPeer(unsigned int id) const
	{
		TestPeer * newPeer = new TestPeer();
		this->constructedPeers[id] = newPeer;

		return newPeer;
	}

	TestPeer * getPeer(unsigned int id) const
	{
		assert(id < maxPeers);
		return this->constructedPeers[id];
	}
private:
	mutable TestPeer * constructedPeers[maxPeers];	
};

bool virtualInvoked = false;

class TestVirtualPeer: public Peer
{
public:
    ~TestVirtualPeer()
	{
		virtualInvoked = true;
	}

    void accept(Peer & peer)
	{
		virtualInvoked = true;
	}
};

class TestVirtualPeerFactory: public PeerFactory
{
public:
    ~TestVirtualPeerFactory()
	{
		virtualInvoked = true;
	}

    Peer * constructPeer(unsigned int id) const
	{
		virtualInvoked = true;
		return 0;
	}
};

class TestVirtualWakeupSequence: public WakeupSequence
{
public:
    ~TestVirtualWakeupSequence()
	{
		virtualInvoked = true;
	}

    Topology::iterator nextPeer()
	{
		virtualInvoked = true;
		return Topology::iterator();
	}

    Topology::iterator begin() const
	{
		return Topology::iterator();
	}

    Topology::iterator end() const
	{
		return Topology::iterator();
	}
};

int main()
{
#if TEST_NUM == 10
	{
		TestPeerFactory testPeerFactory;
		TestPeer * peer0 = static_cast<TestPeer *>(testPeerFactory.constructPeer(0));
		TestPeer * peer1 = static_cast<TestPeer *>(testPeerFactory.constructPeer(1));
		TestPeer * peer2 = static_cast<TestPeer *>(testPeerFactory.constructPeer(2));

		cout << "Test: wakeup with empty neighbour list" << endl;
		peer0->wakeup();

		cout << "Test: removing unexisting connection from a peer" << endl;
		peer0->removePeer(peer1);

		cout << "Test: adding connection to a peer" << endl;
		peer0->addPeer(peer1);
		peer0->addPeer(peer2);
		peer0->wakeup();
		assert(peer1->wasVisited());
		assert(peer2->wasVisited());
		peer1->reset();
		peer2->reset();

		cout << "Test: removing connection from a peer" << endl;
		peer0->addPeer(peer1);
		peer0->addPeer(peer2);
		peer0->removePeer(peer1);
		peer0->wakeup();
		assert(!peer1->wasVisited());
		assert(peer2->wasVisited());

		delete peer0;
		delete peer1;
		delete peer2;
	}
#endif
#if TEST_NUM == 20
	{
		TestPeerFactory testPeerFactory;
		TestPeer * peer0 = static_cast<TestPeer *>(testPeerFactory.constructPeer(0));
		TestPeer * peer1 = static_cast<TestPeer *>(testPeerFactory.constructPeer(1));
		TestPeer * peer2 = static_cast<TestPeer *>(testPeerFactory.constructPeer(1));

		Topology t;

		cout << "Test: adding peer to a topology" << endl;
		t.addPeer(1, peer1);
		t.addPeer(0, peer0);
		Topology::iterator peerIt = t.begin();
		assert(t.getPeer(peerIt) == peer0);
		++peerIt;
		assert(t.getPeer(peerIt) == peer1);
		++peerIt;
		assert(peerIt == t.end());

		cout << "Test: adding peer with the same id to a topology" << endl;
		t.addPeer(1, peer2);
		peerIt = t.begin();
		assert(t.getPeer(peerIt) == peer0);
		++peerIt;
		assert(t.getPeer(peerIt) == peer2);
		++peerIt;
		assert(peerIt == t.end());

		cout << "Test: removing peer from a topology" << endl;
		t.removePeer(1);
		peerIt = t.begin();
		assert(t.getPeer(peerIt) == peer0);
		++peerIt;
		assert(peerIt == t.end());

		cout << "Test: removing unexistig peer from a topology" << endl;
		t.removePeer(2);
		peerIt = t.begin();
		assert(t.getPeer(peerIt) == peer0);
		++peerIt;
		assert(peerIt == t.end());
	}
#endif
#if TEST_NUM == 30
	{
		TestPeerFactory testPeerFactory;
		TestPeer * peer0 = static_cast<TestPeer *>(testPeerFactory.constructPeer(0));
		TestPeer * peer1 = static_cast<TestPeer *>(testPeerFactory.constructPeer(1));
		TestPeer * peer2 = static_cast<TestPeer *>(testPeerFactory.constructPeer(2));
		Topology t;		

		t.addPeer(0, peer0);
		t.addPeer(1, peer1);
		t.addPeer(2, peer2);

		cout << "Test: adding connection between unexisting peers" << endl;
		t.addPeerConnection(3, 7);
		t.addPeerConnection(10, 34);

		cout << "Test: adding connection between existing peers" << endl;
		t.addPeerConnection(1, 0);
		t.addPeerConnection(1, 2);
		peer0->wakeup();
		peer1->wakeup();
		peer2->wakeup();
		assert(peer0->wasVisited());
		assert(!peer1->wasVisited());
		assert(peer2->wasVisited());
		peer0->reset();
		peer1->reset();
		peer2->reset();

		cout << "Test: addig existing connection" << endl;	
		t.addPeerConnection(1, 0);
		peer0->wakeup();
		peer1->wakeup();
		peer2->wakeup();
		assert(peer0->wasVisited());
		assert(!peer1->wasVisited());
		assert(peer2->wasVisited());
		peer0->reset();
		peer1->reset();
		peer2->reset();		

		cout << "Test: adding connection to unexisting peer" << endl;
		t.addPeerConnection(1, 7);
		peer0->wakeup();
		peer1->wakeup();
		peer2->wakeup();
		assert(peer0->wasVisited());
		assert(!peer1->wasVisited());
		assert(peer2->wasVisited());
		peer0->reset();
		peer1->reset();
		peer2->reset();

		cout << "Test: removing connection between unexisting peers" << endl;
		t.removePeerConnection(3, 7);
		t.removePeerConnection(10, 34);

		cout << "Test: removing connection to unexisting peer" << endl;
		t.removePeerConnection(1, 7);
		peer0->wakeup();
		peer1->wakeup();
		peer2->wakeup();
		assert(peer0->wasVisited());
		assert(!peer1->wasVisited());
		assert(peer2->wasVisited());
		peer0->reset();
		peer1->reset();
		peer2->reset();

		cout << "Test: removing existig connection" << endl;
		t.removePeerConnection(1, 0);
		peer0->wakeup();
		peer1->wakeup();
		peer2->wakeup();
		assert(!peer0->wasVisited());
		assert(!peer1->wasVisited());
		assert(peer2->wasVisited());
		peer0->reset();
		peer1->reset();
		peer2->reset();

		cout << "Test: removing unexisting connection" << endl;
		t.removePeerConnection(1, 0);
		peer0->wakeup();
		peer1->wakeup();
		peer2->wakeup();
		assert(!peer0->wasVisited());
		assert(!peer1->wasVisited());
		assert(peer2->wasVisited());
		peer0->reset();
		peer1->reset();
		peer2->reset();
	}
#endif
#if TEST_NUM == 40
	{
		TestPeerFactory testPeerFactory;
		Topology * t;

		cout << "Test: Ring topology of size 0 test." << endl;
		t = createRingTopology(testPeerFactory, 0);
		assert(t->begin() == t->end());
		delete t;

		cout << "Test: Ring topology of size 1 test." << endl;
		t = createRingTopology(testPeerFactory, 1);
		Topology::iterator peerIt = t->begin();
		assert(t->getPeer(peerIt) != 0);
		assert(t->getPeer(peerIt) == testPeerFactory.getPeer(0));
		t->getPeer(peerIt)->wakeup();
		assert(testPeerFactory.getPeer(0)->wasVisited());
		++peerIt;
		assert(peerIt == t->end());
		delete t;

		cout << "Test: Ring topology of size 2 test." << endl;
		t = createRingTopology(testPeerFactory, 2);
		peerIt = t->begin();
		assert(t->getPeer(peerIt) != 0);
		assert(t->getPeer(peerIt) == testPeerFactory.getPeer(0));
		t->getPeer(peerIt)->wakeup();
		assert(!testPeerFactory.getPeer(0)->wasVisited());
		assert(testPeerFactory.getPeer(1)->wasVisited());
		testPeerFactory.getPeer(1)->reset();
		++peerIt;
		assert(t->getPeer(peerIt) != 0);
		assert(t->getPeer(peerIt) == testPeerFactory.getPeer(1));
		t->getPeer(peerIt)->wakeup();
		assert(testPeerFactory.getPeer(0)->wasVisited());
		assert(!testPeerFactory.getPeer(1)->wasVisited());
		++peerIt;
		assert(peerIt == t->end());
		delete t;
	}
#endif
#if TEST_NUM == 50
	{
		cout << "Test: AvgCalcPeerSinglePeakFactory && AvgCalcPeer" << endl;
		AvgCalcPeerSinglePeakFactory peerFactory;
		Peer * peer0 = peerFactory.constructPeer(0);
		Peer * peer1 = peerFactory.constructPeer(1);
		AvgCalcPeer * avgCalcPeer0 = dynamic_cast<AvgCalcPeer *>(peer0);
		AvgCalcPeer * avgCalcPeer1 = dynamic_cast<AvgCalcPeer *>(peer1);
		assert(avgCalcPeer0 != 0);
		assert(avgCalcPeer1 != 0);
		assert(avgCalcPeer0->getValue() == 1.f);
		assert(avgCalcPeer1->getValue() == 0.f);
		avgCalcPeer0->accept(*avgCalcPeer1);
		assert(avgCalcPeer0->getValue() == 0.5f);
		assert(avgCalcPeer1->getValue() == 0.5f);
	}
#endif
#if TEST_NUM == 51
	{
		TestPeerFactory testPeerFactory;
		TestPeer * peer0 = static_cast<TestPeer *>(testPeerFactory.constructPeer(0));
		TestPeer * peer1 = static_cast<TestPeer *>(testPeerFactory.constructPeer(1));
		TestPeer * peer2 = static_cast<TestPeer *>(testPeerFactory.constructPeer(2));
		TestPeer * peer3 = static_cast<TestPeer *>(testPeerFactory.constructPeer(3));
		Topology t;

		cout << "Test: LinearWakeupSequence" << endl;
		t.addPeer(1, peer1);
		t.addPeer(0, peer0);
		t.addPeer(2, peer2);
		LinearWakeupSequence sequence(t.begin(), t.end());
		Topology::iterator peerIt = sequence.begin();
		assert(t.getPeer(peerIt) == peer0);
		peerIt = sequence.nextPeer();
		assert(t.getPeer(peerIt) == peer1);
		peerIt = sequence.nextPeer();
		assert(t.getPeer(peerIt) == peer2);
		t.addPeer(3, peer3);
		peerIt = sequence.nextPeer();
		assert(t.getPeer(peerIt) == peer3);
		peerIt = sequence.nextPeer();
		assert(peerIt == sequence.end());
	}
#endif
#if TEST_NUM == 60
	{
		cout << "Test: global method simulate" << endl;
		TestPeerFactory testPeerFactory;
		Topology * t = createRingTopology(testPeerFactory, 5);
		for (unsigned int j = 0; j < 3; j++)
		{
			LinearWakeupSequence sequence(t->begin(), t->end());
			simulate(*t, sequence);
			for (unsigned int i = 0; i < 5; i++)
			{
				assert(testPeerFactory.getPeer(i)->wasVisited());
				testPeerFactory.getPeer(i)->reset();
			}
		}
		delete t;
	}
#endif
#if TEST_NUM == 70
	{
		cout << "Test: no possible Peer object creation" << endl;
		Peer peer;
	}
#endif
#if TEST_NUM == 71
	{
		cout << "Test: no possible WakeupSequence object creation" << endl;
		WakeupSequence wakeupSequence;
	}
#endif
#if TEST_NUM == 72
	{
		cout << "Test: no possible PeerFactory object creation" << endl;
		PeerFactory peerFactory;
	}
#endif
#if TEST_NUM == 80
	{
		cout << "Test: virtual functions" << endl;

		Peer * peer = new TestVirtualPeer;
		TestVirtualPeer testVirtualPeer;
		peer->accept(testVirtualPeer);
		assert(virtualInvoked);
		virtualInvoked = false;
		delete peer;
		assert(virtualInvoked);
		virtualInvoked = false;

		PeerFactory * peerFactory = new TestVirtualPeerFactory;
		peerFactory->constructPeer(0);
		assert(virtualInvoked);
		virtualInvoked = false;
		delete peerFactory;
		assert(virtualInvoked);
		virtualInvoked = false;

		WakeupSequence * wakeupSequence = new TestVirtualWakeupSequence;
		wakeupSequence->nextPeer();
		assert(virtualInvoked);
		virtualInvoked = false;
		delete wakeupSequence;
		assert(virtualInvoked);
	}
#endif

	assert(TestPeer::objectsInLive == 0);
	
	return 0;
}
