cmake_minimum_required(VERSION 3.28)
project(GenericGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GRAPHICAL_PATH "${CMAKE_SOURCE_DIR}/../libs/Graphical")

if (EXISTS ${GRAPHICAL_PATH}/CMakeLists.txt)
    add_subdirectory(${GRAPHICAL_PATH} ${CMAKE_BINARY_DIR}/GraphicalBuild)
else()
    message(FATAL_ERROR "Graphical library not found at: ${GRAPHICAL_PATH}")
endif()

add_executable(GenericGame
    "src/main.cpp"
    "src/Game.cpp"
    "src/Character.cpp"
    "src/Utilities/PathHelper.cpp"
)

target_link_libraries(GenericGame PRIVATE Graphical)
