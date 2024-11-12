cmake_minimum_required(VERSION 3.28)
project(IsoMaker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
include(raylib.cmake)
include(SDL2.cmake)

add_executable(IsoMaker 
    "../src/main.cpp"
    "../src/Input/Keyboard.cpp"
    "../src/Input/Mouse.cpp"
    "../src/Input/Controller.cpp"
)

target_link_libraries(IsoMaker PRIVATE raylib)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)
