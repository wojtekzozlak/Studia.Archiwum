make clean && make latc-jvm


FILTERS="good/core"
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

  ./latc-jvm $test Global > Global.j 2> /dev/null
  if [ 0 -ne $? ]
  then
    echo -e "\e[0;31m[COMPILATION ERROR]\e[0m"
    continue
  fi

  java -jar external/jasmin.jar Global.j > /dev/null
  if [ 0 -ne $? ]
  then
    echo -e "\e[0;31m[JASMIN ERROR]\e[0m"
    continue
  fi

  java Global < $INPUT > output
  if [ 0 -ne $? ]
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

