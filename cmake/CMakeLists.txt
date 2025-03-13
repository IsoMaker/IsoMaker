cmake_minimum_required(VERSION 3.28)
project(IsoMaker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

option(BUILD_TESTS "Build unit tests" OFF)

include(FetchContent)
include(raylib.cmake)
include(raygui.cmake)
include(SDL2.cmake)

if (NOT BUILD_TESTS)
    add_executable(IsoMaker
            "../src/main.cpp"
            "../src/Editor/2D/2DEditor.cpp"
            "../src/Input/Keyboard.cpp"
            "../src/Input/Mouse.cpp"
            "../src/Input/Controller.cpp"
            "../src/Editor/3DMap/3DMapEditor.cpp"
            "../src/Assets/AAsset.cpp"
            "../src/Assets/Asset2D.cpp"
            "../src/Assets/Asset3D.cpp"
            "../src/MainUI/MainUI.cpp"
            "../src/Object2D/BasicObject2D.cpp"
            "../src/Object3D/BasicObject3D.cpp"
            "../src/Render/Camera.cpp"
            "../src/Render/Window.cpp"
            "../src/Utilities/DrawCubeTexture.cpp"
            "../src/Utilities/ObjectBox.cpp"
            "../src/Utilities/Vector.cpp"
    )
    target_link_libraries(IsoMaker PRIVATE raylib raygui SDL2)
endif()

if (BUILD_TESTS)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/refs/heads/main.zip
    )
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    add_executable(tests
            "../tests/test_main.cpp"
    )
    target_link_libraries(tests PRIVATE gtest gtest_main raylib raygui SDL2)

    add_test(NAME IsoMakerTests COMMAND tests)
endif()
