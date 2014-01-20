make clean && make


FILTERS="good/core|extensions/arrays|extensions/struct|extensions/objects|special"
DIR="lattests/"

TESTS=`find $DIR -name "*.lat" | grep -E $FILTERS | sort`
for test in $TESTS
do
  echo -n "TEST: ${test:${#DIR}}	" 
  # is input out there?
  INPUT=${test:0:${#test}-4}.input
  OUTPUT=${test:0:${#test}-4}.output

  if [ ! -e $INPUT ]
  then
    INPUT="/dev/null"
  fi

  ./latc-x86 $test > test.s 2> /dev/null
  if [ 0 -ne $? ]
  then
    echo -e "\e[0;31m[COMPILATION ERROR]\e[0m"
    continue
  fi

  gcc -m32 -O0 test.s bin/x86/runtime.o -o test > /dev/null
  if [ 0 -ne $? ]
  then
    echo -e "\e[0;31m[GCC ERROR]\e[0m"
    continue
  fi

  NAME=`basename ${test:0:${#test}-4}`
  ./test < $INPUT > output
  if [ 0 -ne $? -a $NAME != error ]
  then
    echo -e "\e[0;31m[RUNTIME ERROR?]\e[0m"
    continue
  fi

  diff -q output $OUTPUT > /dev/null
  if [ 0 -eq $? ]
  then
    echo -e "\e[0;32m[OK]\e[0m"
  else
    echo -e "\e[0;31m[BAD OUTPUT]\e[0m"
    continue
  fi
done

