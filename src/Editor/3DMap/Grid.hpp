/*
** EPITECH PROJECT, 2025
** IsoMaker
** File description:
** Grid
*/

#pragma once

#include <raylib.h>
#include "rlgl.h"

class MapGrid {
    public:
        MapGrid(int gridSize = 22, int cellSize = 1, Color gridColor = MAGENTA, Color backgroundColor = BLACK);
        ~MapGrid();

        void init();

        void draw();
    protected:
        void drawMesh();
        void drawXAxis(float x, float y, float z);
        void drawZAxis(float x, float y, float z);
        void drawLines(float gridExtent, float y, float offsets);
        void drawWireframe();

        int _gridSize;
        int _cellSize;
        Color _gridColor;
        Color _gridBackgroundColor;
        Mesh _gridMesh;
        Model _gridModel;
};
