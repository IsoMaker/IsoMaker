cmake_minimum_required(VERSION 3.28)
project(IsoMaker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
include(raylib.cmake)
include(raygui.cmake)
include(SDL2.cmake)


add_executable(IsoMaker
    "../src/main.cpp"
    "../src/Editor/2D/2DEditor.cpp"
)

target_link_libraries(IsoMaker PRIVATE raylib raygui SDL2)
