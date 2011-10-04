CC = gcc
CFLAGS = -Wall -levent -pthread -D WAITING_FOR=4000
TARGETS = client server_poll server_libevent server_fork

all: $(TARGETS) 

clean:
	rm -f $(TARGETS) 
