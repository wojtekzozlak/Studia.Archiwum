CC = gcc
CFLAGS = -Wall
TARGETS = s_client1 s_server2 thread_server 

all: $(TARGETS) 
s_server2: s_server2.c err.c err.h
s_client1: s_client1.c
thread_server: thread_server.c
	$(CC) $(CFLAGS) $^ -o $@ -lpthread
clean:
	rm -f $(TARGETS) 
