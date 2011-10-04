CXX = g++
CXX_CFLAGS = -Wall -levent -pthread
TARGETS = client server

all:
	$(CXX) server.cc -o server $(CXX_CFLAGS)
	$(CXX) client.cc -o client $(CXX_CFLAGS)
	

clean:
	rm -f $(TARGETS) 
