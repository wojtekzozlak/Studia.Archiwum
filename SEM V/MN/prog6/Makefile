
CC = gcc
CFLAGS = -g -Wall -pedantic
OBJECTS =
LIBS = -lm

TARGET = prog6

default: $(TARGET)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $*.o

prog6: prog6.c
	$(CC) $(CFLAGS) prog6.c $(OBJECTS) $(LIBS) -o prog6

clean:
	rm -f *.o

