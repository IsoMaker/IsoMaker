/*
** EPITECH PROJECT, 2025
** IsoMaker
** File description:
** Grid
*/

#pragma once

#include <raylib.h>
#include "rlgl.h"
#include <optional>

#include "Utilities/Vector.hpp"

using namespace Utilities;

namespace map {
    class MapGrid {
        public:
            MapGrid(int cellAmount = 21, int cellSize = 1, Color gridColor = (Color){130, 130, 130, 200}, Color backgroundColor = WHITE);
            ~MapGrid();

            void init();

            void draw();

            std::optional<Vector3D> getCellFromRay(const Ray &ray) const;
        protected:
        private:
            void drawMesh();
            void drawXAxis(float x, float y, float z);
            void drawZAxis(float x, float y, float z);
            void drawLines(float gridExtent, float y, float offsets);
            void drawWireframe();

            int _cellAmount;
            int _cellSize;
            Color _color;
            Color _backgroundColor;
            Mesh _mesh;
            Model _model;
            float _scale;
    };
}
