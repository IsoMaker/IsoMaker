#include "AWindow.hpp"

AWindow::AWindow(int x, int y, int width, int height)
    : x(x), y(y), width(width), height(height) {}

void AWindow::Resize(int x, int y, int width, int height) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
}
