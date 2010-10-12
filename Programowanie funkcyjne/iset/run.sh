#!/bin/bash
g++ test.cpp -o gen
./gen > test.ml
ocamlc -c test.ml
ocamlc -o test iSet.cmo test.cmo
