#/***********************************************
# * Zaawansowane Systemy Operacyjne
# *
# * Pierwsze zadanie (2012).
# *
# * Autor: Marcin Osowski, nr indeksu 292682
# *
# **********************************************/

#
# DEBUGLEVEL = 0..10
# 0 means turn off debugging

DEBUGLEVEL=9

COBJS=test.o err.o
TARGET_LIB=libtest.so
TARGET=test
TARGET_LIBS=-L. -lcall_cnt -ltest

CC=gcc
LD=gcc
BASE_COPTS=-m32 -Wall -Werror -pthread -fPIC -D ENABLE_VT100 -g 
BASE_LDOPTS=-m32 -pthread -ldl

ifdef TEST
	TEST_CASE=-D TEST_CASE=$(TEST)
else
	TEST_CASE=
endif

ifndef THREADS_NUM
	THREADS_NUM=1
endif


ifeq ($(DEBUGLEVEL),0)
	COPTS=$(BASE_COPTS) -O2 -g0 -D DEBUGLEVEL=0 -D NDEBUG=1
	LDOPTS=$(BASE_LDOPTS)
else
	COPTS=$(BASE_COPTS) -O0 -g -D DEBUGLEVEL=$(DEBUGLEVEL)
	LDOPTS=$(BASE_LDOPTS)
endif


all: $(TARGET)

libcall_cnt.so: ../libcall_cnt.so
	cp $< $@

$(TARGET): clean  $(COBJS) $(TARGET_LIB) libcall_cnt.so
	$(CC) $(COPTS) -o $@ $(TARGET_LIBS) $(COBJS)
	LD_LIBRARY_PATH="." LD_BIND_NOW="1" ./test

$(TARGET_LIB): libtest.o
	$(LD) $(LDOPTS) -shared -Wl,-soname,$@ -o $@ libtest.o

%.o: %.c
	$(CC) $(COPTS) $(TEST_CASE) -D THREADS_NUM=$(THREADS_NUM) -c $<

.PHONY:	all, clean, package, test

package: clean
	rm -f ../marcin-testy.tar
	cd ..; tar cf marcin-testy.tar marcin-testy/

clean:
	rm -f *~
	rm -f *.o
	rm -f *.so
	rm -f $(TARGET) $(TARGET_LIB)


