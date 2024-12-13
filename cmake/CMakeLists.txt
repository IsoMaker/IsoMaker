cmake_minimum_required(VERSION 3.28)
project(IsoMaker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

include(FetchContent)
include(raylib.cmake)
include(raygui.cmake)
include(SDL2.cmake)

# Définir l'exécutable principal
add_executable(IsoMaker
        "../src/main.cpp"
        "../src/Editor/2D/2DEditor.cpp"
)

target_link_libraries(IsoMaker PRIVATE raylib raygui SDL2)

# Ajouter Google Test pour les tests
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

# Créer un exécutable pour les tests
enable_testing()
add_executable(IsoMakerTests
        "../tests/test_main.cpp"
        "../src/Editor/2D/2DEditor.cpp" # Inclure les fichiers nécessaires pour les tests
)

target_link_libraries(IsoMakerTests PRIVATE gtest gtest_main raylib raygui SDL2)

# Ajouter une commande pour exécuter les tests
include(GoogleTest)
gtest_discover_tests(IsoMakerTests)
