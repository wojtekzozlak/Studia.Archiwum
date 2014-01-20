make latc


FILTERS="good"
DIR="lattests/"

TESTS=`find $DIR -name "*.lat" | grep -E $FILTERS | sort`
for test in $TESTS
do
  echo -n "TEST: ${test:${#DIR}}	" 
  ./latc < $test &> /dev/null
  if [ 0 -eq $? ]
  then
    echo -e "\e[0;32m[OK]\e[0m"
  else
    echo -e "\e[0;31m[ERROR]\e[0m"
  fi 
done


FILTERS="bad"
DIR="lattests/"

TESTS=`find $DIR -name "*.lat" | grep -E $FILTERS | sort`
for test in $TESTS
do
  echo -n "TEST: ${test:${#DIR}}	" 
  ./latc < $test &> /dev/null
  if [ 0 -ne $? ]
  then
    echo -e "\e[0;32m[OK]\e[0m"
  else
    echo -e "\e[0;31m[ERROR]\e[0m"
  fi 
done

