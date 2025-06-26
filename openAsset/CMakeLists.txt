cmake_minimum_required(VERSION 3.28)
project(OpenAsset LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_TESTS "Build unit tests" OFF)

set(GRAPHICAL_PATH "${CMAKE_SOURCE_DIR}/../libs/Graphical")

if (EXISTS ${GRAPHICAL_PATH}/CMakeLists.txt)
    add_subdirectory(${GRAPHICAL_PATH} ${CMAKE_BINARY_DIR}/GraphicalBuild)
else()
    message(FATAL_ERROR "Graphical library not found at: ${GRAPHICAL_PATH}")
endif()

if (NOT BUILD_TESTS)
    add_executable(OpenAsset
        "main.cpp"
    )

    target_link_libraries(OpenAsset PRIVATE
        Graphical
    )

    # Add include directories for header files
    target_include_directories(OpenAsset PRIVATE
        "${CMAKE_SOURCE_DIR}/../../libs/Graphical/includes"
        "${CMAKE_SOURCE_DIR}/../../libs/Graphical/src"
    )
endif()
