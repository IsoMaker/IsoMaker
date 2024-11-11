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

        float _redParams = 255;
        float _greenParams = 0;
        float _blueParams = 0;

        ToolType _currentTool = ToolType::Pen;

        Editor(unsigned int screenWidth, unsigned int screenHeight);
        ~Editor() {};

        void drawGrid();
        void drawPixels();
        void paintPixel(int x, int y);
        void customizationTools();
        void setTool(ToolType tool);
    };

}

inline bool operator==(const Color &firstColor, const Color &secondColor) {
    return firstColor.r == secondColor.r && firstColor.g == secondColor.g &&
           firstColor.b == secondColor.b && firstColor.a == secondColor.a;
}
