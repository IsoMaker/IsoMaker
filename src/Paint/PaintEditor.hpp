#pragma once

#include "raylib.h"
#include "raygui.h"
#include <vector>

namespace paint {

    enum class ToolType {
        Pen,
        Eraser,
        Pipette
    };

    class Editor {
    public:
        unsigned int _screenWidth;
        unsigned int _screenHeight;
        int _gridSize = 20;
        Color _currentColor;
        std::vector<std::vector<Color>> pixelColors;
        float _zoomLevel = 1.0f;
        const float _zoomStep = 0.1f;
        const int _originalGridSize = 20;

        float _redParams = 255;
        float _greenParams = 0;
        float _blueParams = 0;

        int _canvasOffsetX = 0;
        int _canvasOffsetY = 0;

        ToolType _currentTool = ToolType::Pen;

        Editor(unsigned int screenWidth, unsigned int screenHeight)
            : _screenWidth(screenWidth), _screenHeight(screenHeight),
              pixelColors(screenWidth / _gridSize, std::vector<Color>(screenHeight / _gridSize, WHITE)),
              _canvasOffsetX((screenWidth - (pixelColors.size() * _gridSize)) / 2),
              _canvasOffsetY((screenHeight - (pixelColors[0].size() * _gridSize)) / 2) {}
        ~Editor() {};

        void update();
        void drawGrid();
        void drawPixels();
        void handlePixel(int x, int y);
        void customizationTools();
        void zoomIn();
        void zoomOut();

        void setTool(ToolType tool) {
            _currentTool = tool;
        }

    };

}

inline bool operator==(const Color &firstColor, const Color &secondColor)
{
    return firstColor.r == secondColor.r && firstColor.g == secondColor.g &&
           firstColor.b == secondColor.b && firstColor.a == secondColor.a;
}
