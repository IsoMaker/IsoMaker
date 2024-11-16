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
    "../src/Input/AHandler.cpp"
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
