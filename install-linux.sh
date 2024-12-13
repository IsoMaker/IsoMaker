#!/bin/bash

rm -f bin/IsoMaker
cmake -B cmake/build -S cmake
cmake --build cmake/build

if [ -f cmake/build/IsoMaker ]; then
    mv cmake/build/IsoMaker bin
else
    echo "Compilation failed: IsoMaker binary not found!"
    exit 1
fi
