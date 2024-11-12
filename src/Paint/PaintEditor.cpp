#include "PaintEditor.hpp"
#include <iostream>

paint::Editor::Editor(unsigned int screenWidth, unsigned int screenHeight)
    : _screenWidth(screenWidth), _screenHeight(screenHeight),
      pixelColors(screenWidth / _gridSize, std::vector<Color>(screenHeight / _gridSize, WHITE)) {}

void paint::Editor::setTool(ToolType tool) {
    _currentTool = tool;
}

void paint::Editor::drawGrid() {
    int cols = _screenWidth / _gridSize;
    int rows = _screenHeight / _gridSize;

    for (int i = 0; i <= cols; i++) {
        DrawLine(i * _gridSize, 0, i * _gridSize, _screenHeight, LIGHTGRAY);
    }
    for (int j = 0; j <= rows; j++) {
        DrawLine(0, j * _gridSize, _screenWidth, j * _gridSize, LIGHTGRAY);
    }
}

void paint::Editor::drawPixels() {
    for (int i = 0; i < pixelColors.size(); i++) {
        for (int j = 0; j < pixelColors[i].size(); j++) {
            DrawRectangle(i * _gridSize, j * _gridSize, _gridSize, _gridSize, pixelColors[i][j]);
        }
    }
}

void paint::Editor::handlePixel(int x, int y) {
    int col = x / _gridSize;
    int row = y / _gridSize;

    if (col >= 0 && col < pixelColors.size() && row >= 0 && row < pixelColors[0].size()) {
        switch (_currentTool) {
            case ToolType::Pen:
                pixelColors[col][row] = _currentColor;
                break;
            case ToolType::Eraser:
                pixelColors[col][row] = WHITE;
                break;
            case ToolType::Pipette:
                _currentColor = pixelColors[col][row];
                _redParams = _currentColor.r;
                _greenParams = _currentColor.g;
                _blueParams = _currentColor.b;
                break;
        }
    }
}

void paint::Editor::customizationTools() {
    DrawText("Custom Color Picker", 20, 480, 10, DARKGRAY);
    GuiSliderBar((Rectangle){ 20, 490, 200, 20 }, "0", "255", &_redParams, 0, 255);
    GuiSliderBar((Rectangle){ 20, 520, 200, 20 }, "0", "255", &_greenParams, 0, 255);
    GuiSliderBar((Rectangle){ 20, 550, 200, 20 }, "0", "255", &_blueParams, 0, 255);

    _currentColor = { (unsigned char)_redParams, (unsigned char)_greenParams, (unsigned char)_blueParams, 255 };
    DrawRectangle(250, 520, 50, 50, _currentColor);

    if (GuiButton((Rectangle){ 320, 490, 60, 30 }, "Pen")) {
        setTool(ToolType::Pen);
    }
    if (GuiButton((Rectangle){ 320, 520, 60, 30 }, "Eraser")) {
        setTool(ToolType::Eraser);
    }
    if (GuiButton((Rectangle){ 320, 550, 60, 30 }, "Pipette")) {
        setTool(ToolType::Pipette);
    }
}

void paint::Editor::zoomIn()
{
    if (_zoomLevel <= 1.5) {
        _zoomLevel += _zoomStep;
        _gridSize = static_cast<int>(_originalGridSize * _zoomLevel);
    }
}

void paint::Editor::zoomOut()
{
    if (_zoomLevel >= 0.6) {
        _zoomLevel -= _zoomStep;
        _gridSize = static_cast<int>(_originalGridSize * _zoomLevel);
    }
}
