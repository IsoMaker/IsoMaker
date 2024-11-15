#pragma once

#include "raylib.h"
#include "raygui.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

namespace paint {

    enum class ToolType {
        Pen,
        Eraser,
        Pipette
    };

    class Editor {
    public:
        Editor(unsigned int screenWidth, unsigned int screenHeight)
            : _screenWidth(screenWidth), _screenHeight(screenHeight),
              _currentColor(RED),
              pixelColors(screenWidth / _gridSize, std::vector<Color>(screenHeight / _gridSize, BLANK)) {
                updateCanvasOffset();
              }

        ~Editor() {}

        void update();
        void drawGrid();
        void drawPixels();
        void handlePixel(int x, int y);
        void customizationTools();
        void zoomIn();
        void zoomOut();

        void saveToFile(const std::string& filename);
        void loadFromFile(const std::string& filename);


        ToolType getTool() const { return _currentTool; }
        Color getCurrentColor() const { return _currentColor; }
        float getZoomLevel() const { return _zoomLevel; }
        int getGridSize() const { return _gridSize; }

        void setTool(ToolType tool) { _currentTool = tool; }
        void setCurrentColor(const Color& color) { _currentColor = color; }
        void setZoomLevel(float zoomLevel) {
            if (zoomLevel >= 0.5f && zoomLevel <= 2.0f) {
                _zoomLevel = zoomLevel;
                _gridSize = _originalGridSize * _zoomLevel;
                updateCanvasOffset();
            }
        }

    private:
        unsigned int _screenWidth;
        unsigned int _screenHeight;

        int _gridSize = 20;
        Color _currentColor;
        float _zoomLevel = 1.0f;
        const float _zoomStep = 0.1f;
        const int _originalGridSize = 20;
        int _canvasOffsetX = 0;
        int _canvasOffsetY = 0;
        ToolType _currentTool = ToolType::Pen;

        std::vector<std::vector<Color>> pixelColors;

        void updateCanvasOffset() {
            _canvasOffsetX = (_screenWidth - (pixelColors.size() * _gridSize)) / 2;
            _canvasOffsetY = (_screenHeight - (pixelColors[0].size() * _gridSize)) / 2;
        }
    };

}
