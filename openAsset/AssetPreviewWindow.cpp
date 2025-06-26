#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <iostream>

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(400, 300, "Asset Preview");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (GuiButton({ 50, 60, 120, 30 }, "Button 1")) {
            std::cout << "[AssetWindow] Button 1 clicked" << std::endl;
        }

        if (GuiButton({ 50, 110, 120, 30 }, "Button 2")) {
            std::cout << "[AssetWindow] Button 2 clicked" << std::endl;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
