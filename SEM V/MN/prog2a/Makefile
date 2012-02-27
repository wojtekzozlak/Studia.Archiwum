
CC = gcc
CFLAGS = -g -Wall -pedantic -L.

LIBS = -lclapack -lcblas -lF77 -lm

BSLIBS = -lpsout -lmultibs -lpkgeom -lpknum -lpkvaria

TARGET = prog2

default: $(TARGET)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $*.o

prog2: prog2.c
	$(CC) $(CFLAGS) -o prog2 prog2.c $(LIBS)

clean:
	rm -f *.o

mrproper:
	rm -f *.o $(TARGET)

