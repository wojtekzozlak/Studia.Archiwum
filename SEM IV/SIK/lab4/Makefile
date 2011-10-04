CXX = g++
CXX_CFLAGS = -Wall -levent -pthread -lsctp
TARGETS = client server client2

all:
	$(CXX) server.cc -o server $(CXX_CFLAGS)
	$(CXX) client.cc -o client $(CXX_CFLAGS)
	$(CXX) client2.cc -o client2 $(CXX_CFLAGS)
	

clean:
	rm -f $(TARGETS) 
