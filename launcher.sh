#!/bin/sh
T=`mktemp`
tail -n +5 "$0"|xz -d|g++ -O3 -std=c++20 -pthread -march=native -xc++ -o$T -
(sleep 3;rm $T)&exec $T