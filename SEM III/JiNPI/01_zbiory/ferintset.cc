#include "ferintset.h"
#include "intset.h"
#include <iostream>
using namespace std;

#ifdef DEBUG
  const bool debug = true;
#else
  const bool debug = false;
#endif

static unsigned long init()
{
  if(debug) cerr << "ferintset init invoked" << endl;
  unsigned long id = intset_new();
  intset_insert(id, 3);
  intset_insert(id, 5);
  intset_insert(id, 17);
  intset_insert(id, 257);
  intset_insert(id, 65537);
  if(debug) cerr << "ferintset init finished" << endl;
  return id;
}

const unsigned long ferintset = init();
