#!/bin/sh

make clean c4ke c4ke-mini-bench || exit 1

NODES=$(./c4ke bench | awk '{v=$1}END{print v}')
NODES_MINI=$(./c4ke-mini-bench bench | awk '{v=$1}END{print v}')

if [ "$NODES" != "$NODES_MINI" ]
then
    echo "Minified bench $NODES_MINI does not match unminified bench $NODES"
    exit 1
fi

make c4ke-tcec || exit 1

SIZE=$(du -b c4ke-tcec | awk '{print $1}')

echo "size: $SIZE"
echo "bench: $NODES"