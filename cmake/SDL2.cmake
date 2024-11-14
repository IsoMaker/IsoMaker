message(STATUS "Install SDL2...")

find_package(SDL2 2.0 QUIET)

if (NOT SDL2_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            SDL2
            URL https://github.com/libsdl-org/SDL/archive/refs/tags/release-2.0.14.zip
    )
    FetchContent_GetProperties(SDL2)
    if (NOT SDL2_POPULATED)
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(SDL2)
        set(SDL_STATIC ON CACHE BOOL "" FORCE)
        add_subdirectory(${SDL2_SOURCE_DIR} ${SDL2_BINARY_DIR})
    endif()
endif()
