message(STATUS "Install jsoncpp PACKAGES...")

FetchContent_Declare(
    jsoncpp
    GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
    GIT_TAG master
)
set(JSONCPP_WITH_TESTS OFF CACHE BOOL "" FORCE)
set(JSONCPP_WITH_POST_BUILD_UNITTEST OFF CACHE BOOL "" FORCE)
set(JSONCPP_USE_SECURE_MEMORY OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(jsoncpp)
