#!/bin/bash

set -eu

mkdir -p bin
echo '*' > bin/.gitignore
g++-10 -std=c++11 -O2 -Wall src/run.cpp -o bin/g10
time bin/g10