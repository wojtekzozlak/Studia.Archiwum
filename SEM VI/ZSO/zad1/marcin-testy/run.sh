#!/bin/bash

set -e

ALL_CASES="1 2 3 4 5 101 102 103 104 105 201 202"
CASES=$ALL_CASES

THREADS_NUM=200

for CASE in $CASES; do
    make THREADS_NUM=$THREADS_NUM TEST=$CASE test
done

echo
echo "Test cases '$CASES' OK"
