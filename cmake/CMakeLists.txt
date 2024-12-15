cmake_minimum_required(VERSION 3.28)
project(IsoMaker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)

if(WIN32)
    set(CMAKE_GENERATOR_TOOLSET "v143")
    add_compile_options(-Wall)
else()
    add_compile_options(-Wall -Wextra)
endif()

include(FetchContent)
include(glad.cmake)
include(raylib.cmake)
include(raygui.cmake)
include(SDL2.cmake)

add_executable(IsoMaker
    "../src/main.cpp"
    "../src/Editor/2D/2DEditor.cpp"
)

#target_link_libraries(IsoMaker PRIVATE raylib raygui SDL2-static glad)
#target_link_libraries(IsoMaker PRIVATE raylib raygui SDL2 glad)
