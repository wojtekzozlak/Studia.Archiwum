CXX = g++
OBJS = intset.o ferintset.o

debuglevel = 0

ifeq ($(debuglevel),1)
  CXXFLAGS = -Wall -O0 -ggdb -DDEBUG
else
  CXXFLAGS = -Wall -O2
endif


all:  $(OBJS)

intset.o: intset.h intset.cc
	g++ $(CXXFLAGS) -c intset.cc -o intset.o

ferintset.o: ferintset.h intset.h ferintset.cc
	g++ $(CXXFLAGS) -c ferintset.cc -o ferintset.o

clean:
	rm -f *.o *~

