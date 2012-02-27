
CC = gcc
CFLAGS = -g -Wall -pedantic
OBJECTS = postscript.o
LIBS = -llapack -lblas -lgfortran -lm

TARGET = prog5

default: $(TARGET)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $*.o

prog5: prog5.c postscript.h postscript.o
	$(CC) $(CFLAGS) prog5.c $(OBJECTS) -L. $(LIBS) -o prog5

clean:
	rm -f *.o

