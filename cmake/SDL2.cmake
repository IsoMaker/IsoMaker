message(STATUS "Install SDL2 PACKAGES...")

find_package(SDL2 CONFIG QUIET)

if (NOT SDL2_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        SDL2
        URL https://github.com/libsdl-org/SDL/releases/download/release-2.28.4/SDL2-2.28.4.tar.gz
    )
    FetchContent_MakeAvailable(SDL2)
endif()

if(WIN32)
    if(TARGET SDL2-static)
        message(STATUS "Using SDL2-static")
    else()
        message(FATAL_ERROR "Failed to configure SDL2")
    endif()
endif()
