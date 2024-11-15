#include "PaintEditor.hpp"
#include <iostream>

void paint::Editor::update()
{
    if (IsKeyPressed(KEY_UP))
        zoomIn();
    else if (IsKeyPressed(KEY_DOWN))
        zoomOut();

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        Vector2 mousePos = GetMousePosition();
        handlePixel(mousePos.x, mousePos.y);
    }

    drawPixels();
    drawGrid();
    customizationTools();
}

void paint::Editor::drawGrid()
{
    int cols = pixelColors.size();
    int rows = pixelColors[0].size();

    for (int i = 0; i <= cols; i++) {
        int x = _canvasOffsetX + i * _gridSize;
        DrawLine(x, _canvasOffsetY, x, _canvasOffsetY + rows * _gridSize, LIGHTGRAY);
    }
    for (int j = 0; j <= rows; j++) {
        int y = _canvasOffsetY + j * _gridSize;
        DrawLine(_canvasOffsetX, y, _canvasOffsetX + cols * _gridSize, y, LIGHTGRAY);
    }
}

void paint::Editor::drawPixels()
{
    for (int i = 0; i < pixelColors.size(); i++) {
        for (int j = 0; j < pixelColors[i].size(); j++) {
            int x = _canvasOffsetX + i * _gridSize;
            int y = _canvasOffsetY + j * _gridSize;
            DrawRectangle(x, y, _gridSize, _gridSize, pixelColors[i][j]);
        }
    }
}

void paint::Editor::handlePixel(int x, int y)
{
    int col = (x - _canvasOffsetX) / _gridSize;
    int row = (y - _canvasOffsetY) / _gridSize;

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

void paint::Editor::customizationTools()
{
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
    if (_zoomLevel < 2.0f) {
        _zoomLevel += _zoomStep;
        _gridSize = _originalGridSize * _zoomLevel;

        _canvasOffsetX = (_screenWidth - (pixelColors.size() * _gridSize)) / 2;
        _canvasOffsetY = (_screenHeight - (pixelColors[0].size() * _gridSize)) / 2;
    }
}

void paint::Editor::zoomOut()
{
    if (_zoomLevel > 0.5f) {
        _zoomLevel -= _zoomStep;
        _gridSize = _originalGridSize * _zoomLevel;

        _canvasOffsetX = (_screenWidth - (pixelColors.size() * _gridSize)) / 2;
        _canvasOffsetY = (_screenHeight - (pixelColors[0].size() * _gridSize)) / 2;
    }
}
