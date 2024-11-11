#pragma once

#include "raylib.h"
#include "raygui.h"
#include <vector>

namespace paint {

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

            Editor(unsigned int screenWidth, unsigned int screenHeight);
            ~Editor() {};

            void draw_grid();
            void draw_pixels();
            void paint_pixel(int x, int y);
            void color_selector();

    };

}

inline bool operator==(const Color &firstColor, const Color &secondColor) {
    return firstColor.r == secondColor.r && firstColor.g == secondColor.g &&
           firstColor.b == secondColor.b && firstColor.a == secondColor.a;
}
