CC = gcc
CFLAGS = -Wall -ansi --pedantic -c
LDFLAGS = -Wall -ansi --pedantic

OBJECTS = err.o
ALL = zad2 proc_fork proc_tree proc_exec simple_shell

all: $(ALL)

%.o : %.c
	$(CC) $(CFLAGS) $<

$(ALL) : % : %.o $(OBJECTS)       
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o $(ALL) *~
