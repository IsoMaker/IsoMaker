#include "paint_tools.hpp"

paint_tools::paint_tools(unsigned int screenWidth, unsigned int screenHeight)
    : _screenWidth(screenWidth), _screenHeight(screenHeight),
      pixelColors(screenWidth / _gridSize, std::vector<Color>(screenHeight / _gridSize, WHITE)) {}

void paint_tools::draw_grid()
{
    int cols = _screenWidth / _gridSize;
    int rows = _screenHeight / _gridSize;

    for (int i = 0; i <= cols; i++) {
        DrawLine(i * _gridSize, 0, i * _gridSize, _screenHeight, LIGHTGRAY);
    }
    for (int j = 0; j <= rows; j++) {
        DrawLine(0, j * _gridSize, _screenWidth, j * _gridSize, LIGHTGRAY);
    }
}

void paint_tools::draw_pixels()
{
    for (int i = 0; i < pixelColors.size(); i++) {
        for (int j = 0; j < pixelColors[i].size(); j++) {
            DrawRectangle(i * _gridSize, j * _gridSize, _gridSize, _gridSize, pixelColors[i][j]);
        }
    }
}

void paint_tools::paint_pixel(int x, int y)
{
    int col = x / _gridSize;
    int row = y / _gridSize;

    if (col >= 0 && col < pixelColors.size() && row >= 0 && row < pixelColors[0].size()) {
        if (_currentColor == pixelColors[col][row]) {
            pixelColors[col][row] = WHITE;
        } else {
            pixelColors[col][row] = _currentColor;
        }
    }
}
