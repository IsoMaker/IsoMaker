message(STATUS "Installing raygui PACKAGE...")

if (NOT TARGET raylib)
    set(raylib_DIR "${raylib_BINARY_DIR}")
endif()

find_path(RAYGUI_INCLUDE_DIR raygui.h)

if (NOT RAYGUI_INCLUDE_DIR)
    include(FetchContent)
    FetchContent_Declare(
        raygui
        URL https://github.com/raysan5/raygui/archive/refs/tags/4.0.tar.gz
    )
    FetchContent_GetProperties(raygui)
    if (NOT raygui_POPULATED)
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(raygui)
        add_library(raygui INTERFACE)
        target_include_directories(raygui INTERFACE ${raygui_SOURCE_DIR}/src)
        target_link_libraries(raygui INTERFACE raylib)
    endif()
else()
    message(STATUS "Found raygui in system include directories.")
    add_library(raygui INTERFACE)
    target_include_directories(raygui INTERFACE ${RAYGUI_INCLUDE_DIR})
    target_link_libraries(raygui INTERFACE raylib)
endif()
