
CC = gcc
CFLAGS = -g -Wall -pedantic

LIBS = -llapack -lblas -lgfortran -lm

TARGET = prog2

default: $(TARGET)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $*.o

prog2: prog2.c
	$(CC) $(CFLAGS) -o prog2 prog2.c -L. $(LIBS)

clean:
	rm -f *.o

mrproper:
	rm -f *.o $(TARGET)

