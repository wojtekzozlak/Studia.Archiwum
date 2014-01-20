main:
	cd proto && protoc --cpp_out=./ operations.proto
	cd proto && g++ operations.pb.cc -c
	g++ server.cc -c
	g++ engine.cc server.o proto/operations.pb.o -lprotobuf -lpthread -O4 -std=c++0x -funroll-loops -o exec_plan -march=native

clean:
	rm exec_plan *.o
