#include <cstdlib>
#include <cstdio>
#include <iostream>
#include "p2pSimulator.h"
using namespace std;
int main()
{
AvgCalcPeerSinglePeakFactory f;
Topology* t = createRingTopology(f, 20);
LinearWakeupSequence l(t->begin(), t->end());

srand(42);
for(int i = 0; i < 42; i++)
	t->addPeerConnection(rand() % 20, rand() % 20);
for(int i = 0; i < 42; i++)
	t->removePeerConnection(rand() % 20, rand() % 20);

for(int i = 0; i < 57; i++)
	simulate(*t, l);

Topology::iterator it = t->begin();

for(; it != t->end(); ++it)
	printf("%.3lf ", ((AvgCalcPeer*)t->getPeer(it))->getValue());
	
std::cout << std::endl;

return 0;
}
