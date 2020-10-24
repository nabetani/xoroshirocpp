#!/bin/bash

set -eu

mkdir -p bin
echo '*' > bin/.gitignore
opts="-O2 -Wall \
    -I/usr/local/include \
    -L/usr/local/lib"

gcc-10 -c -std=c99 src/eval.c ${opts} -o bin/eval.o
g++-10 -std=c++11 src/eval_xs128.cpp bin/eval.o -o bin/eval_xs128 ${opts} -ltestu01 -lprobdist -lmylib -lgmp -lm
bin/eval_xs128
