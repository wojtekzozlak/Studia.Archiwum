CC=gcc
LD=gcc
LIBS= -lpcap
OPTS= -Wall -pedantic

all: stat pierwszy enty stat_b pierwszy_b enty_b

stat: stat.c
	$(CC) $(OPTS) -o stat stat.c $(LIBS)
stat_b: stat_b.c
	$(CC) $(OPTS) -o stat_b stat_b.c $(LIBS)

pierwszy: pierwszy.c
	$(CC) $(OPTS) -o pierwszy pierwszy.c $(LIBS)
pierwszy_b: pierwszy_b.c
	$(CC) $(OPTS) -o pierwszy_b pierwszy_b.c $(LIBS)

enty: enty.c
	$(CC) $(OPTS) -o enty enty.c $(LIBS)
enty_b: enty_b.c
	$(CC) $(OPTS) -o enty_b enty_b.c $(LIBS)

clean:
	rm stat pierwszy enty stat_b pierwszy_b enty_b
