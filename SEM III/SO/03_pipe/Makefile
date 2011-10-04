all: child_pipe parent_pipe parent_dup test_fifo


test_fifo: test_fifo.o err.o
	cc -Wall -o test_fifo test_fifo.o err.o

child_pipe: child_pipe.o err.o
	cc -Wall -o child_pipe child_pipe.o err.o

parent_pipe: parent_pipe.o err.o
	cc -Wall -o parent_pipe parent_pipe.o err.o

parent_dup: parent_dup.o err.o
	cc -Wall -o parent_dup parent_dup.o err.o

test_fifo.o: test_fifo.c err.h
	cc -Wall -c test_fifo.c

child_pipe.o: child_pipe.c err.h
	cc -Wall -c child_pipe.c

parent_pipe.o: parent_pipe.c err.h
	cc -Wall -c parent_pipe.c

parent_dup.o: parent_dup.c err.h
	cc -Wall -c parent_dup.c

err.o: err.c err.h
	cc -Wall -c err.c

clean:
	rm -f *.o child_pipe parent_pipe parent_dup test_fifo

