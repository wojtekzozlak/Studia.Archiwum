g++ -c rational.cc -o rational.o
for i in `seq 101 151`;
do
	g++ rational_test.cc rational.o -o test -DTEST_NUM=$i
	echo "test $i"
	./test
	echo ""
done
