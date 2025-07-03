project(Graphical)
cmake_minimum_required(VERSION 3.28)

include(FetchContent)
include(../raylib.cmake)
include(../raygui.cmake)
include(../glad.cmake)

file(GLOB GRAPHICAL_SOURCES
    "src/Assets/Asset3D.cpp"
    "src/Assets/Asset2D.cpp"
    "src/Entities/Character.cpp"
    "src/Entities/MapElement.cpp"
    "src/Input/Gamepad.cpp"
    "src/Input/MouseKeyboard.cpp"
    "src/Render/Camera.cpp"
    "src/Render/PointLight.cpp"
    "src/Render/Window.cpp"
    "src/Utilities/DrawCubeTexture.cpp"
    "src/Utilities/ObjectBox.cpp"
    "src/Utilities/Vector.cpp"
)

add_library(Graphical STATIC ${GRAPHICAL_SOURCES})
target_link_libraries(Graphical PUBLIC
    raylib
    raygui
    glad
)

target_include_directories(Graphical PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/includes
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${glad_SOURCE_DIR}/include
)
