ATARGETS= s_client1a s_server1a
BTARGETS= s_client1b s_server1b

all: a b
a: $(ATARGETS)
b: $(BTARGETS)

s_server1a: s_server1a.c
	gcc -Wall $^ -o $@

s_client1a: s_client1a.c
	gcc -Wall $^ -o $@

s_server1b: s_server1b.c
	gcc -Wall $^ -o $@

s_client1b: s_client1b.c
	gcc -Wall $^ -o $@

clean:
	rm -f $(ATARGETS) $(BTARGETS)
