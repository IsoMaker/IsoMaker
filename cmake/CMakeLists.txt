cmake_minimum_required(VERSION 3.28)
project(IsoMaker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
include(raylib.cmake)

add_executable(IsoMaker ../src/main.cpp)

target_link_libraries(IsoMaker PRIVATE raylib)
