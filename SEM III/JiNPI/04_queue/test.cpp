#include <iostream>
#include <exception>
#include <cassert>

#include "PriorityQueue.hh"

int main() {
PriorityQueue<int, int> Q;
assert(Q.empty());

std::cout << "a1" << std::endl;

Q.insert(1, 42);
Q.insert(2, 13);

std::cout << "a2" << std::endl;

assert(Q.size() == 2);
assert(Q.maxKey() == 1);
assert(Q.maxValue() == 42);
assert(Q.minKey() == 2);
assert(Q.minValue() == 13);

std::cout << "a3" << std::endl;

Q.deleteMax();
Q.deleteMin();
Q.deleteMin();

assert(Q.empty());

std::cout << "a4" << std::endl;

Q.insert(1, 100);
Q.insert(2, 100);
Q.insert(3, 300);

std::cout << "a5" << std::endl;

try {
Q.changeValue(4, 400);
} catch (PriorityQueueNotFoundException const & pqnfex) {
  std::cout << pqnfex.what() << std::endl;
} catch (...) {
assert(! "exception missing!");
}

std::cout << "a6" << std::endl;

cout << "teraz wazny changeValue" << endl;
Q.changeValue(2, 200);

std::cout << "a7" << std::endl;

try {
while (true) {
std::cout << Q.minValue() << std::endl;
Q.deleteMin();
}
assert(! "Q.minValue() on empty Q did not throw!");
} catch (PriorityQueueEmptyException const & pqeex) {
std::cout << pqeex.what() << std::endl;
} catch (...) {
assert(! "exception missing!");
}

std::cout << "a8" << std::endl;

try {
PriorityQueue<int, int>::key_type k = Q.minKey();
assert(! "Q.minKey() on empty Q did not throw!");
} catch (std::exception const & ex) {
std::cout << ex.what() << std::endl;
} catch (...) {
assert(! "exception missing!");
}

std::cout << "a9" << std::endl;

PriorityQueue<int, int> T;
T.insert(1, 1);
T.insert(2, 4);
Q.insert(3, 9);
Q.insert(4, 16);
Q.merge(T);
assert(Q.size() == 4);
assert(Q.minValue() == 1);
assert(Q.maxValue() == 16);
assert(T.empty());

std::cout << "a10" << std::endl;

PriorityQueue<int, int> S = Q;
swap(Q, T);
assert(T == S);

std::cout << "a11" << std::endl;

std::cout << "ALL OK!" << std::endl;
return 0;
}
