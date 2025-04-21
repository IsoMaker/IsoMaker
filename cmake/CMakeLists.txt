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
        "../src/Input/Keyboard.cpp"
        "../src/Input/Mouse.cpp"
        "../src/Input/Controller.cpp"
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
        "../src/UI/UIManager.cpp"
        "../src/UI/UIComponents.cpp"
        "../src/UI/RayguiImpl.cpp"
        "../src/UI/ImGuiImpl.cpp"
    )

    target_link_libraries(IsoMaker PRIVATE
        Graphical
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
