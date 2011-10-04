TARGETS= client server server2

all: $(TARGETS)

server: server.c
	gcc -Wall $^ -o $@

server2: server2.c
	gcc -Wall $^ -o $@

client: client.c
	gcc -Wall $^ -o $@

clean:
	rm -f $(TARGETS)
