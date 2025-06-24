#!/bin/bash

rm -f libGraphical.a
cmake -B build -S .
cmake --build build

if [ -f build/libGraphical.a ]; then
    mv build/libGraphical.a .
else
    echo "Compilation failed: libGraphical.a binary not found!"
    exit 1
fi
