CXX = gcc
CXXFLAGS = -std=gnu99 -pedantic -Wall -O2 -fPIC

fifo: fifo.c strategy.h pagesim.c pagesim.h page.h
	$(CXX) $(CXXFLAGS) -c fifo.c -o strategy.o
	$(CXX) $(CXXFLAGS) -c pagesim.c -o pagesim.o
	$(CXX) $(CXXFLAGS) -shared -o libpagesim.so pagesim.o strategy.o -lrt

lfu: lfu.c strategy.h pagesim.c pagesim.h page.h
	$(CXX) $(CXXFLAGS) -c lfu.c -o strategy.o
	$(CXX) $(CXXFLAGS) -c pagesim.c -o pagesim.o
	$(CXX) $(CXXFLAGS) -shared -o libpagesim.so pagesim.o strategy.o -lrt

clean:
	rm -f *.o
	rm -f *.so
