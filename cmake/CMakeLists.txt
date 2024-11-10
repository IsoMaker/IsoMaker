cmake_minimum_required(VERSION 3.28)
project(IsoMaker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
include(raylib.cmake)
include(SDL2.cmake)

add_executable(IsoMaker
    "../src/main.cpp"
    "../src//paint/paint_tools.cpp"
)

target_link_libraries(IsoMaker PRIVATE raylib)
target_link_libraries(IsoMaker PRIVATE SDL2)
