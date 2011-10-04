#include <set>
#include <map>
#include <iostream>
#include <algorithm>
#include <vector>
#include "intset.h"
using namespace std;

typedef map< unsigned long, set<int> > SetMap;
#ifdef DEBUG
  const bool debug = true;
#else
  const bool debug = false;
#endif

SetMap& sets()
{
  static SetMap s;
  return s;
} 

unsigned long& nextId()
{
  static unsigned long next = 0;
  return next;
}

unsigned long intset_new()
{  
  if(debug) cerr << "intset_new()" << endl;
  set<int> newSet;
  unsigned int id = nextId()++;
  sets().insert(make_pair(id, newSet));
  if(debug) cerr << "intset_new: set " << id << " created" << endl;
  return id;
}

void intset_delete(unsigned long id)
{
  if(debug) cerr << "intset_delete(" << id << ")" << endl;
  SetMap::iterator it = sets().find(id);
  if(it != sets().end())
  {
    sets().erase(it);
    if(debug) cerr << "intset_delete: set " << id << " deleted" << endl;
  } else if(debug) cerr << "intset_delete: set " << id
                        << " does not exist" << endl;
}

void intset_insert(unsigned long id, int elem)
{
  if(debug) cerr << "intset_insert(" << id << ", " << elem << ")" << endl;
  SetMap::iterator it = sets().find(id);
  if(it != sets().end())
  {
    it->second.insert(elem);
    if(debug) cerr << "intset_insert: set " << id << ", element " << elem
                   << " inserted " << endl;
  }
  else if(debug) cerr << "intset_insert: set " << id
                      << " does not exist" << endl;
}

void intset_remove(unsigned long id, int elem)
{
  if(debug) cerr << "intset_remove(" << id << ", " << elem << ")" << endl;
  SetMap::iterator it = sets().find(id);
  if(it != sets().end())
  {
    set<int>::iterator elIt = it->second.find(elem);
    if(elIt != it->second.end())
    {
      it->second.erase(elIt);
      if(debug) cerr << "intset_remove: set " << id << ", element " << elem
                     << " removed " << endl;
    } else if(debug) cerr << "intset_remove: set " << id
                          << " does not contain element " << elem << endl;
  }
  else if(debug) cerr << "intset_remove: set " << id
                      << " does not exist" << endl;  
}

int intset_find(unsigned long id, int elem)
{
  if(debug) cerr << "intset_find(" << id << ", " << elem << ")" << endl;
  SetMap::iterator it = sets().find(id);
  if(it != sets().end())
  {
    set<int>::iterator elIt = it->second.find(elem);
    if(elIt != it->second.end())
    {
      if(debug) cerr << "intset_find: set " << id << " contains element "
                     << elem << endl;
      return 1;
    }
    else if(debug) cerr << "intset_find: set " << id
                        << " does not contain element " << elem << endl;
  }
  else if(debug) cerr << "intset_find: set " << id << " does not exist" << endl;
  return 0;
}

size_t intset_size(unsigned long id)
{
  if(debug) cerr << "intset_size(" << id << ")" << endl;
  SetMap::iterator it = sets().find(id);
  if(it != sets().end())
  {
    unsigned long size = it->second.size();
    if(debug) cerr << "intset_size: set " << id << " contains "
                   << size << " elements" << endl;
    return size;
  }
  else if(debug) cerr << "intset_size: set " << id << " does not exist" << endl;
  return 0;
}

void intset_union(unsigned long id1, unsigned long id2, unsigned long id3)
{
  if(debug) cerr << "intset_union("
                 << id1 << ", " << id2 << ", " << id3
                 << ")" << endl;
  SetMap::iterator set1 = sets().find(id1),
                   set2 = sets().find(id2),
                   set3 = sets().find(id3);
  if(debug)
    if(set1 == sets().end() || set2 == sets().end() || set3 == sets().end())
    {
      cerr << "intset_union: set "
           << (set2 == sets().end() ? (set1 == sets().end() ? id1 : id2) : id3)
           << " does not exits" << endl;
      return ;
    }
  set<int> newSet;
  set_union(set2->second.begin(), set2->second.end(),
            set3->second.begin(), set3->second.end(),
            inserter(newSet, newSet.begin()));
  swap(newSet, set1->second);
}

void intset_difference(unsigned long id1, unsigned long id2, unsigned long id3)
{
  if(debug) cerr << "intset_difference("
                 << id1 << ", " << id2 << ", " << id3
                 << ")" << endl;
  SetMap::iterator set1 = sets().find(id1),
                   set2 = sets().find(id2),
                   set3 = sets().find(id3);
  if(debug)
    if(set1 == sets().end() || set2 == sets().end() || set3 == sets().end())
    {
      cerr << "intset_difference: set "
           << (set2 == sets().end() ? (set1 == sets().end() ? id1 : id2) : id3) 
           << " does not exits" << endl;
      return ;
    }
  set<int> newSet;
  set_difference(set2->second.begin(), set2->second.end(),
                 set3->second.begin(), set3->second.end(),
                 inserter(newSet, newSet.begin()));
  swap(newSet, set1->second);
}

