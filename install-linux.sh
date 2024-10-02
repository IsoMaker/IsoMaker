rm -f bin/IsoMaker
cmake cmake -B cmake/build
make -C cmake/build
mv cmake/build/IsoMaker bin