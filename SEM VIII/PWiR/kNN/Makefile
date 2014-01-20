CPPFLAGS = -Wall -I /opt/cuda-5.0.35/include/
LDFLAGS	= -L /usr/lib64/nvidia
LDLIBS = -lOpenCL -lstdc++

EXECS = knn knn_brute gen knn_1 knn_2 knn_3

TARGET: $(EXECS)

knn_brute:
	g++  $(CPPFLAGS) $(LDFLAGS)  knn.cpp $(LDLIBS) -DKERNEL_NAME=\"bruteKernel\" -DPASS_CONSTANTS -o knn_brute

knn_1:
	g++  $(CPPFLAGS) $(LDFLAGS)  knn.cpp $(LDLIBS) -DKERNEL_NAME=\"withConstants\" -o knn_1

knn_2:
	g++  $(CPPFLAGS) $(LDFLAGS)  knn.cpp $(LDLIBS) -DKERNEL_NAME=\"queryInRegisters\" -o knn_2

knn_3:
	g++  $(CPPFLAGS) $(LDFLAGS)  knn.cpp $(LDLIBS) -DKERNEL_NAME=\"heap\" -o knn_3

knn:
	g++  $(CPPFLAGS) $(LDFLAGS)  knn.cpp $(LDLIBS) -DKERNEL_NAME=\"heap\" -DLOCAL_THREADS=128 -o knn

.PHONY: clean TARGET
clean:
	rm -f $(EXECS) *.o *~ 
