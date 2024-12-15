message(STATUS "Install OpenGL PACKAGES...")

if (NOT GLAM_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        glad
        URL https://github.com/Dav1dde/glad/archive/refs/tags/v0.1.36.tar.gz
    )
    FetchContent_MakeAvailable(glad)
endif()
