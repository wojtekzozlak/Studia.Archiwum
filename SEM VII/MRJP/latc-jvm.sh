#!/bin/bash

TEST=$1
NAME=`basename $TEST`
NAME=${NAME:0:${#NAME}-4}
JASMIN=$NAME.j

./latc-jvm $TEST $NAME 1> $JASMIN
java -jar external/jasmin.jar $JASMIN

