project(Graphical)
cmake_minimum_required(VERSION 3.28)

file(GLOB GRAPHICAL_SOURCES
    "src/Assets/AAsset.cpp"
    "src/Assets/Asset3D.cpp"
    "src/Assets/Asset2D.cpp"
    "src/Entities/BasicObject.cpp"
    "src/Input/Gamepad.cpp"
    "src/Input/MouseKeyboard.cpp"
    "src/Render/Camera.cpp"
    "src/Render/Window.cpp"
    "src/Utilities/Vector.cpp"
    "src/Utilities/DrawCubeTexture.cpp"
    "src/Utilities/ObjectBox.cpp"
)

add_library(Graphical STATIC ${GRAPHICAL_SOURCES})
target_link_libraries(Graphical PUBLIC raylib)

# Ensure the compiler can find headers from "includes/" and "src/"
target_include_directories(Graphical PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)




    # recreate this in a library but for now lets copy paste ugly ew side
    # "../src/Render/Camera.cpp"
    # "../src/Render/Window.cpp"
    # "../src/Utilities/Vector.cpp"
    # "../src/Assets/AAsset.cpp"
    # "../src/Assets/Asset3D.cpp"
    # "../src/Assets/Asset2D.cpp"
    # "../src/Utilities/DrawCubeTexture.cpp"
    # "../src/Utilities/ObjectBox.cpp"
    # "../src/Object3D/BasicObject3D.cpp"
    # "../src/Object2D/BasicObject2D.cpp"
