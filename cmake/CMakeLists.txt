cmake_minimum_required(VERSION 3.28)
project(IsoMaker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_TESTS "Build unit tests" OFF)

set(GRAPHICAL_PATH "${CMAKE_SOURCE_DIR}/../libs/Graphical")

if (EXISTS ${GRAPHICAL_PATH}/CMakeLists.txt)
    add_subdirectory(${GRAPHICAL_PATH} ${CMAKE_BINARY_DIR}/GraphicalBuild)
else()
    message(FATAL_ERROR "Graphical library not found at: ${GRAPHICAL_PATH}")
endif()

if (NOT BUILD_TESTS)
    add_executable(IsoMaker
        "../src/main.cpp"
        "../src/Editor/3DMap/3DMapEditor.cpp"
        "../src/Editor/3DMap/Grid.cpp"
        "../src/MainUI/MainUI.cpp"
        "../src/UI/UIManager.cpp"
        "../src/UI/UIComponents.cpp"
        "../src/UI/RayguiImpl.cpp"
    )

    target_link_libraries(IsoMaker PRIVATE
        Graphical
    )
    
    # Add include directories for header files
    target_include_directories(IsoMaker PRIVATE
        "${CMAKE_SOURCE_DIR}/../libs/Graphical/includes"
        "${CMAKE_SOURCE_DIR}/../libs/Graphical/src"
    )
endif()

if (BUILD_TESTS)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
    )
    FetchContent_Populate(googletest)

    enable_testing()

    add_executable(tests
        "../tests/test_main.cpp"
    )

    target_link_libraries(tests PRIVATE
        gtest
        gtest_main
    )

    add_test(NAME IsoMakerTests COMMAND tests)
endif()
