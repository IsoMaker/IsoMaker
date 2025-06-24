#!/bin/bash
set -e

echo "===== IsoMaker Installation Script for macOS ====="

if [ ! -d "bin" ]; then
    echo "Creating bin directory..."
    mkdir -p bin
fi

echo "Checking dependencies..."

if ! command -v cmake &> /dev/null; then
    echo "Error: cmake not found. Please install cmake."
    echo "You can install it with: brew install cmake"
    exit 1
fi

if ! command -v clang++ &> /dev/null; then
    echo "Error: clang++ not found. Please install Xcode Command Line Tools."
    echo "You can install it with: xcode-select --install"
    exit 1
fi

echo "Building IsoMaker..."
rm -f bin/IsoMaker
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