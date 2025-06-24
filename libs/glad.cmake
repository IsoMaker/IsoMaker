FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v0.1.36
)
FetchContent_MakeAvailable(glad)

# Define glad_SOURCE_DIR if needed
set(glad_SOURCE_DIR ${glad_SOURCE_DIR} CACHE PATH "Path to glad source")
