#include "paint/paint_tools.hpp"

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 600;
    paint_tools paint(screenWidth, screenHeight);
    Vector2 mousePos;

    InitWindow(screenWidth, screenHeight, "Pixel Art Editor");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            mousePos = GetMousePosition();
            paint.paint_pixel(mousePos.x, mousePos.y);
        }
        BeginDrawing();
        ClearBackground(RAYWHITE);
        paint.draw_pixels();
        paint.draw_grid();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
