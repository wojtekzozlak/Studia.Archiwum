# The compiler can be replaced to mpicc.
CC          := mpicc
CFLAGS      := -Wall -Werror -O3 -c
LFLAGS      := -Wall -O3 -lm
ALL         := laplace-seq.exe laplace-par.exe

all : $(ALL)


%.exe : %.o
	$(CC) $(LFLAGS) -o $@ $<


%.o : %.c laplace-common.h laplace-common-impl.h
	$(CC) $(CFLAGS) $<


clean :
	rm -f *.o $(ALL)

