test: prepare
	./bin/do_test.sh

prepare:
	g++ src/sol.cc -o bin/sol
	g++ src/sorter.cc -o bin/sorter
	g++ src/make_test.cc -o bin/make_test
	cp src/do_test.sh bin/do_test.sh
	chmod 755 bin/do_test.sh

clean:
	rm bin/sol bin/sorter bin/make_test bin/do_test.sh log
	rm out/*
	rm in/*
