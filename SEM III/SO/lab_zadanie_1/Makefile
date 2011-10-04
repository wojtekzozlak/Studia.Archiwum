CXX = gcc
OBJS = vertex nadzorca

quiet = 0

ifeq ($(quiet),1)
  CXXFLAGS = -std=gnu99 -pedantic -Wall -O2 -DQUIET
else
  CXXFLAGS = -std=gnu99 -pedantic -Wall -O2
endif


all:  $(OBJS)

vertex: vertex.c err.o common.o
	$(CXX) $(CXXFLAGS) vertex.c err.o common.o -o vertex

nadzorca: overseer.c err.o common.o
	$(CXX) $(CXXFLAGS) overseer.c err.o common.o -o nadzorca

err.o: err.h err.c
	$(CXX) $(CXXFLAGS) -c err.c -o err.o

common.o: common.h common.c
	$(CXX) $(CXXFLAGS) -c common.c -o common.o

clean:
	rm -f *.o
	rm -f vertex
	rm -f nadzorca
