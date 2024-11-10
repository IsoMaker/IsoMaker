#pragma once

#include "raylib.h"
#include <vector>

class paint_tools {
    public:
        unsigned int _screenWidth;
        unsigned int _screenHeight;
        int _gridSize = 20;
        Color _currentColor = RED;
        std::vector<std::vector<Color>> pixelColors;

        paint_tools(unsigned int screenWidth, unsigned int screenHeight);
        ~paint_tools() {};

        void draw_grid();
        void draw_pixels();
        void paint_pixel(int x, int y);

};

inline bool operator==(const Color &firstColor, const Color &secondColor) {
    return firstColor.r == secondColor.r && firstColor.g == secondColor.g &&
           firstColor.b == secondColor.b && firstColor.a == secondColor.a;
}
