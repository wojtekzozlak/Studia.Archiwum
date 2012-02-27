
CC = gcc
CFLAGS = -g -Wall -pedantic

LIBS = -lm

TARGET = prog1

default: $(TARGET)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $*.o

prog1: prog1.c
	$(CC) $(CFLAGS) -o prog1 prog1.c $(LIBS)

clean:
	rm -f *.o

mrproper:
	rm -f *.o $(TARGET)

