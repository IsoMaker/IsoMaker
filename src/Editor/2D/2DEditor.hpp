/**
* @file 2DEditor.hpp
 * @brief 2D Pixel Art Editor
 * @details This file contains the declaration of the 2D Pixel Art Editor class.
 */


#pragma once

#include "raylib.h"
#include "raygui.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

namespace paint {

    /**
    * @enum Color
    */
    enum class ToolType {
        Pen,
        Eraser,
        Pipette
    };

    /**
    * @class Editor
    * @brief The 2D Pixel Art Editor class.
    * @details This class contains the methods and attributes for the 2D Pixel Art Editor.
    */

    class Editor {
    public:
        /**
        * @brief Construct a new Editor object.
        *
        * @param screenWidth The screen width.
        * @param screenHeight The screen height.
        */
        Editor(unsigned int screenWidth, unsigned int screenHeight)
            : _screenWidth(screenWidth), _screenHeight(screenHeight),
              _currentColor(RED),
              pixelColors(screenWidth / _gridSize, std::vector<Color>(screenHeight / _gridSize, BLANK)) {
                updateCanvasOffset();
              }

        /**
        * @brief Destroy the Editor object.
        */
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


        /** @brief Get the tool. */
        ToolType getTool() const { return _currentTool; }
        /** @brief Get the current color. */
        Color getCurrentColor() const { return _currentColor; }
        /** @brief Get the zoom level. */
        float getZoomLevel() const { return _zoomLevel; }
        /** @brief Get the grid size. */
        int getGridSize() const { return _gridSize; }

        /** @brief Set the tool. */
        void setTool(ToolType tool) { _currentTool = tool; }
        /** @brief Set the current color. */
        void setCurrentColor(const Color& color) { _currentColor = color; }
        /** @brief Set the zoom level. */
        void setZoomLevel(float zoomLevel) {
            if (zoomLevel >= 0.5f && zoomLevel <= 2.0f) {
                _zoomLevel = zoomLevel;
                _gridSize = _originalGridSize * _zoomLevel;
                updateCanvasOffset();
            }
        }

    private:
        /** @brief The screen width, no default value. */
        unsigned int _screenWidth;
        /** @brief The screen height, no default value. */
        unsigned int _screenHeight;

        /** @brief The grid size, default is 20. */
        int _gridSize = 20;
        /** @brief The current color. */
        Color _currentColor;
        /** @brief The zoom level, default is 1.0. */
        float _zoomLevel = 1.0f;
        /** @brief The zoom step, default is 0.1. */
        const float _zoomStep = 0.1f;
        /** @brief The original grid size, default is 20. */
        const int _originalGridSize = 20;
        /** @brief The canvas offset x, default is 0. */
        int _canvasOffsetX = 0;
        /** @brief The canvas offset y, default is 0. */
        int _canvasOffsetY = 0;
        /** @brief The current tool, default is Pen. */
        ToolType _currentTool = ToolType::Pen;

        /** @brief The data structure to store the pixel colors. A 2D vector of Color. */
        std::vector<std::vector<Color>> pixelColors;

        void updateCanvasOffset() {
            _canvasOffsetX = (_screenWidth - (pixelColors.size() * _gridSize)) / 2;
            _canvasOffsetY = (_screenHeight - (pixelColors[0].size() * _gridSize)) / 2;
        }
    };

}
