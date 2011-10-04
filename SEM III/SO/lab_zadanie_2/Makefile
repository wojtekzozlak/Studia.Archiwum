CXX = gcc
OBJS = serwer klient cleaner

quiet = 0

ifeq ($(quiet),1)
  CXXFLAGS = -std=gnu99 -pedantic -Wall -O2
else
  CXXFLAGS = -std=gnu99 -pedantic -Wall -O2 -DDEBUG
endif

all:  $(OBJS)

serwer: server.c err.o common.o
	$(CXX) $(CXXFLAGS) server.c err.o common.o -o serwer -pthread

klient: client.c err.o common.o
	$(CXX) $(CXXFLAGS) client.c err.o common.o -o klient

cleaner: cleaner.c err.o
	$(CXX) $(CXXFLAGS) cleaner.c err.o -o cleaner

err.o: err.h err.c
	$(CXX) $(CXXFLAGS) -c err.c -o err.o

common.o: err.o common.h common.c
	$(CXX) $(CXXFLAGS) -c common.c -o common.o

clean:
	rm -f *.o
	rm -f serwer
	rm -f klient
	rm -f cleaner
