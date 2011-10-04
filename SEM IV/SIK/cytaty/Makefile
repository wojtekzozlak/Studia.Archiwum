CC = g++
CFLAGS = -Wall

all: bin/server_shell bin/client_interface

bin/server: src/server.cc build/database.o build/protocol.o
	$(CC) src/server.cc -o bin/server build/database.o build/protocol.o -lsqlite3 $(CFLAGS)

bin/server_shell: src/server_interface.cc bin/server
	$(CC) src/server_interface.cc -o bin/server_shell build/database.o -lsqlite3 $(CFLAGS)

build/database.o: src/database.cc src/database.h
	$(CC) src/database.cc -c -o build/database.o -lsqlite3 $(CFLAGS)

build/protocol.o: src/protocol.cc src/protocol.h
	$(CC) src/protocol.cc -c -o build/protocol.o $(CFLAGS)

clean:
	rm bin/server build/* bin/client_interface bin/server_shell

bin/client_interface: src/client_gui.cc src/client_gui.h src/client_interface.cc src/client.cc src/client.h build/protocol.o
	qmake-qt4 src/client_gui.pro -o build/Makefile
	make -C build clean
	make -C build
	mv build/client_gui bin/client_interface
