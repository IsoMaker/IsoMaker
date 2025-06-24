#!/bin/bash
set -e

echo "===== IsoMaker Installation Script for Linux ====="

if [ ! -d "bin" ]; then
    echo "Creating bin directory..."
    mkdir -p bin
fi

echo "Checking dependencies..."

if ! command -v cmake > /dev/null; then
    echo "Error: cmake not found. Please install cmake."
    exit 1
fi

if ! command -v g++ > /dev/null; then
    echo "Error: g++ not found. Please install a C++ compiler."
    exit 1
fi

echo "Cleaning previous build directory..."
rm -rf cmake/build

echo "Building IsoMaker..."
cmake -B cmake/build -S cmake
cmake --build cmake/build

if [ -f cmake/build/IsoMaker ]; then
    echo "Build successful!"
    mv cmake/build/IsoMaker bin
    chmod +x bin/IsoMaker
    echo "IsoMaker has been installed to bin/IsoMaker"
    echo "You can run it with: ./bin/IsoMaker"
else
    echo "Compilation failed: IsoMaker binary not found!"
    exit 1
fi
