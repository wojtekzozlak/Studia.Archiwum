main: main.c libcall_cnt.so dynamic.so
	gcc main.c -o main -L. -lcall_cnt -ggdb -rdynamic -ldl -m32 -Wall -ldynamic -Wextra

libcall_cnt.so: call_cnt.c call_cnt.h
	gcc call_cnt.c  -shared -fPIC -o libcall_cnt.so -m32 -Wall -ggdb

dynamic.so: dynamic.c dynamic.h
	gcc -shared -fPIC dynamic.c -o libdynamic.so -m32 -ggdb

clean:
	rm main libdynamic.so libcall_cnt.so
