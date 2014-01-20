#!/bin/bash

TEST=$1
NAME=`basename $TEST`
NAME=${NAME:0:${#NAME}-4}

./latc-x86 $TEST > $NAME.s
if [ 0 -eq $? ]
then
  gcc -m32 $NAME.s bin/x86/runtime.o -o $NAME
fi

