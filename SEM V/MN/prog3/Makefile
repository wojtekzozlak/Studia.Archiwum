
CC = gcc
CFLAGS = -g -Wall -pedantic

LIBS = -lm

TARGET = prog3

default: $(TARGET)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $*.o

prog3: prog3.c
	$(CC) $(CFLAGS) -o prog3 prog3.c $(LIBS)

clean:
	rm -f *.o

mrproper:
	rm -f *.o $(TARGET)

