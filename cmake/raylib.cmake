message(STATUS "Install raylib PACKAGES...")

find_path(OPENGL_INCLUDE_DIR GL/gl.h)
find_package(raylib 5.0 QUIET)

if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raylib
        URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED)
        FetchContent_Populate(raylib)
        set(OPENGL_INCLUDE_DIR ${glad_SOURCE_DIR}/include)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif()
endif()
