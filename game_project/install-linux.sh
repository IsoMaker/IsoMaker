#!/bin/bash

rm -f GenericGame
cmake -B build -S .
cmake --build build

if [ -f build/GenericGame ]; then
    mv build/GenericGame .
else
    echo "Compilation failed: GenericGame binary not found!"
    exit 1
fi
