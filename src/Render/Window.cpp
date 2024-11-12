#include "./Window.hpp"
#include "../Utilities/Vector.hpp"
#include <fstream>

using namespace Utilities;
using namespace Render;

Window::Window() {
}

Window::~Window() {
}

void Window::startwindow(Vector2D size) {
    InitWindow(size.x, size.y, "isomaker");
}

void Window::closeWindow() {
    _windowOpen = false;
    CloseWindow();
}

bool Window::isWindowClosing() {
    return WindowShouldClose();
}

bool Window::isWindowOpen() {
    return _windowOpen;
}

void Window::setWindowSize(Vector2D size) {
    SetWindowSize(size.x, size.y);
}

void Window::startRender() {
    BeginDrawing();
}

void Window::clearBackground() {
    ClearBackground(SKYBLUE);
}

void Window::endRender() {
    EndDrawing();
}

void Window::setFPS(int fps) {
    SetTargetFPS(fps);
}