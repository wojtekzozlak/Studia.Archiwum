
CC = gcc
CFLAGS = -g -Wall -pedantic

LIBS = -lm

TARGET = prog4

default: $(TARGET)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $*.o

prog4: prog4.c
	$(CC) $(CFLAGS) -o prog4 prog4.c $(LIBS)

clean:
	rm -f *.o

mrproper:
	rm -f *.o $(TARGET)

