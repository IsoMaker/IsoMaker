message(STATUS "Install SDL2 PACKAGES...")

find_package(SDL2 CONFIG QUIET)

if (NOT SDL2_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            SDL2
            URL https://github.com/libsdl-org/SDL/releases/download/release-2.28.4/SDL2-2.28.4.tar.gz
    )
    FetchContent_GetProperties(SDL2)
    if (NOT SDL2_POPULATED)
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(SDL2)
        set(SDL_STATIC OFF CACHE BOOL "" FORCE) # Utilise la version dynamique
        set(SDL_SHARED ON CACHE BOOL "" FORCE)
        add_subdirectory(${SDL2_SOURCE_DIR} ${SDL2_BINARY_DIR})
    endif()
endif()
