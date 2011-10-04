// JiNP 5 PriorityQueue - Wojciech Żółtak - 292583

#ifndef _PRIORITYQUEUE_HH_

#define _PRIORITYQUEUE_HH_

// A generic priority queue based on two multisets.
//
// Requirements for strong exception guarantee:
// * type has copy constructor (with any guarantee)
// * type has operator< which has strong guarantee
// * type destructor should not throw

#include <set>
#include <utility>
#include <exception>
#include <boost/shared_ptr.hpp>
#include <iostream>
using namespace std;
using namespace boost;

template<typename T>
struct FSPtrPairComparator
// Throws if comparison of types in pair throws.
{
  bool operator() (const T &a, const T &b) const
  { return *a < *b; }
};
    
template<typename T>
struct SFPtrPairComparator
// Throws if comparison of types in pair throws.
{
  bool operator() (const T &a, const T &b) const
  {
    if( !(a->second < b->second) && !(b->second < a->second) )
      return a->first < b->first;
    else
      return a->second < b->second;
  }
};

class PriorityQueueNotFoundException : public std::exception {};
class PriorityQueueEmptyException : public std::exception {};

template<typename K, typename V>
class PriorityQueue {
  private:
    typedef shared_ptr< pair<K, V> > KeyValuePairPtr;
    typedef multiset<KeyValuePairPtr,
                     FSPtrPairComparator<KeyValuePairPtr> >
            KeyValueMultiset;
    typedef multiset<KeyValuePairPtr,
                     SFPtrPairComparator<KeyValuePairPtr> >
            ValueKeyMultiset;

    KeyValueMultiset keyValueSet;
    ValueKeyMultiset valueKeySet;

  public:
    typedef typename KeyValueMultiset::size_type size_type;
    typedef K key_type;
    typedef V value_type;

    PriorityQueue() : keyValueSet(), valueKeySet() {};
    // Complexity: O(1)
    
    PriorityQueue(PriorityQueue<K, V> const &queue)
    // Complexity: O( queue.size() )
    {
      KeyValueMultiset kvTemp(queue.keyValueSet);
      ValueKeyMultiset vkTemp(queue.valueKeySet);
      keyValueSet.swap(kvTemp);
      valueKeySet.swap(vkTemp);
    }

    PriorityQueue<K, V>& operator=(PriorityQueue<K, V> const &queue)
    // Complexity: O( queue.size() )
    {
      PriorityQueue<K, V> temp(queue);
      temp.swap(*this);
      return *this;
    }
    
    void insert(K const &key, V const &value)
    // Complexity: O( log( size() ) )
    {
      KeyValuePairPtr keyValue(new pair<K, V>(key, value));
      typename KeyValueMultiset::iterator kvIt;
      kvIt = keyValueSet.insert(keyValue);
      try
      { valueKeySet.insert(keyValue); }
      catch(...)
      {
        keyValueSet.erase(kvIt);
        throw ;
      }
    }

    void changeValue(K const &key, V const &value)
    // Complexity: O( log( size() ) )
    {
      KeyValuePairPtr keyValue(new pair<K, V>(key, value));
      typename KeyValueMultiset::iterator kvIt =
          keyValueSet.lower_bound(keyValue);

      // searching current and adjacent elements
      if(kvIt != keyValueSet.begin())
        kvIt--;
      for(int i = 0; kvIt != keyValueSet.end() && i < 3; ++kvIt, i++)
        if(!((*kvIt)->first < key || key < (*kvIt)->first))
          break;
      if(kvIt == keyValueSet.end() ||
         (*kvIt)->first < key || key < (*kvIt)->first)
        throw PriorityQueueNotFoundException();

      PriorityQueue<K, V> temp(*this);
      keyValue = *kvIt;
      try
      {
        keyValueSet.erase(kvIt);
        valueKeySet.erase(valueKeySet.find(keyValue));
        insert(key, value);
      }
      catch(...)
      {
        temp.swap(*this);
        throw ;
      }
      
    }

    void merge(PriorityQueue<K, V> &queue)
    // Complexity: O( queue.size() * log( queue.size() + size() ) )
    {
      if(&queue == this)
        return;
      typename KeyValueMultiset::iterator kvIt;
      PriorityQueue<K, V> temp(*this), emptyPQ;
      temp.keyValueSet.insert(queue.keyValueSet.begin(),
                              queue.keyValueSet.end());
      temp.valueKeySet.insert(queue.valueKeySet.begin(),
                              queue.valueKeySet.end());
      temp.swap(*this);
      queue.swap(emptyPQ);
    }

    void swap(PriorityQueue<K, V> &queue)
    // Complexity: O(1)
    {
      keyValueSet.swap(queue.keyValueSet);
      valueKeySet.swap(queue.valueKeySet);
    }

    bool empty() const
    // Complexity: O(1)
    { return keyValueSet.empty(); }
    
    size_type size() const
    // Complexity: O(1)
    { return keyValueSet.size(); }
    
    const K& minKey() const
    // Complexity: O(1)
    {
      if(empty())
        throw PriorityQueueEmptyException();
      return (*valueKeySet.begin())->first;
    }

    const K& maxKey() const
    // Complexity: O(1)
    {
      if(empty())
        throw PriorityQueueEmptyException();
      return (*valueKeySet.rbegin())->first;
    }

    const V& minValue() const
    // Complexity: O(1)
    {
      if(empty())
        throw PriorityQueueEmptyException();
      return (*valueKeySet.begin())->second;
    }

    const V& maxValue() const
    // Complexity: O(1)
    {
      if(empty())
        throw PriorityQueueEmptyException();
      return (*valueKeySet.rbegin())->second;
    }

    void deleteMin()
    // Complexity: O( log( size() ) )
    {
      if(empty())
        return ;
      typename ValueKeyMultiset::iterator vkIt = valueKeySet.begin();
      KeyValuePairPtr temp = *vkIt;
      keyValueSet.erase(keyValueSet.find(temp));
      valueKeySet.erase(vkIt);
    }

    void deleteMax()
    // Complexity: O( log( size() ) )
    {
      if(empty())
        return ;
      typename ValueKeyMultiset::iterator vkIt = --valueKeySet.end();
      KeyValuePairPtr temp = *vkIt;
      keyValueSet.erase(keyValueSet.find(temp));
      valueKeySet.erase(vkIt);
    }
   
    template<typename KK, typename VV>
    friend bool operator==(const PriorityQueue<K, V> &q1,
                           const PriorityQueue<KK, VV> &q2)
    { return !(q1 < q2 || q2 < q1); }
    
    template<typename KK, typename VV>
    friend bool operator!=(const PriorityQueue<K, V> &q1,
                           const PriorityQueue<KK, VV> &q2)
    { return q1 < q2 || q2 < q1; }
    
    template<typename KK, typename VV>
    friend bool operator<(const PriorityQueue<K, V> &q1,
                          const PriorityQueue<KK, VV> &q2)
    {
      return lexicographical_compare(q1.keyValueSet.begin(),
                                     q1.keyValueSet.end(),
                                     q2.keyValueSet.begin(),
                                     q2.keyValueSet.end(),
                                     q1.keyValueSet.key_comp());
    }

    template<typename KK, typename VV>
    friend bool operator>(const PriorityQueue<K, V> &q1,
                          const PriorityQueue<KK, VV> &q2)
    { return q2 < q1; }
    
    template<typename KK, typename VV>
    friend bool operator<=(const PriorityQueue<K, V> &q1,
                           const PriorityQueue<KK, VV> &q2)
    { return !(q1 > q2); }

    template<typename KK, typename VV>
    friend bool operator>=(const PriorityQueue<K, V> &q1,
                           const PriorityQueue<KK, VV> &q2)
    { return !(q1 < q2); }

};

template<typename K, typename V>
void swap(PriorityQueue<K, V> &q1, PriorityQueue<K, V> &q2)
// Complexity: O(1)
{ q1.swap(q2); }

#endif
