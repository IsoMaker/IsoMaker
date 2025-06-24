cd ..;
echo "building tests for the project...";
cmake -B cmake/build -S cmake -DBUILD_TESTS=ON && cmake --build cmake/build;
echo "--------------------------";
echo "running tests...";
cd cmake/build && ctest --rerun-failed --output-on-failure; cd ../..;
echo "--------------------------";