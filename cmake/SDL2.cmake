message(STATUS "Installing SDL2...")

find_package(SDL2 CONFIG QUIET)

if (NOT SDL2_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        SDL2
        URL https://github.com/libsdl-org/SDL/archive/refs/tags/release-2.30.9.tar.gz
    )
    FetchContent_GetProperties(SDL2)
endif()
