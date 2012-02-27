
CC = gcc
CFLAGS = -g -Wall -pedantic

LIBS = -llapack -lblas -lgfortran -lm

TARGET = zad1

default: $(TARGET)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $*.o

zad1: zad1.c newton
	$(CC) $(CFLAGS) -o zad1 zad1.c -L. $(LIBS) newton.o

newton: newton.c newton.h
	$(CC) $(CFLAGS) -c newton.c

clean:
	rm -f *.o

mrproper:
	rm -f *.o $(TARGET)

