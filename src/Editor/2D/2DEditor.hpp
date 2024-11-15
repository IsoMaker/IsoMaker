/**
 * @file 2DEditor.hpp
 * @brief Declaration of the 2D Pixel Art Editor.
 * @details This file contains the declaration of the Editor class, which handles the creation and manipulation of pixel art images.
 *          It includes tools for drawing, erasing, color selection, grid management, and zooming in/out of the canvas.
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
     * @enum ToolType
     * @brief Defines the available tools in the 2D Pixel Art Editor.
     * @details The tools represent the different actions the user can perform:
     *          Pen for drawing, Eraser for clearing pixels, and Pipette for color picking.
     */
    enum class ToolType {
        Pen,        /**< The pen tool for drawing on the canvas. */
        Eraser,     /**< The eraser tool for removing pixels. */
        Pipette     /**< The pipette tool for picking colors from the canvas. */
    };

    /**
     * @class Editor
     * @brief The main class for the 2D Pixel Art Editor.
     * @details This class provides the core functionality for creating and editing pixel art, including grid management,
     *          pixel manipulation, color customization, and file handling. It also allows for zooming and tool selection.
     */
    class Editor {
    public:
        /**
         * @brief Constructs a new Editor object.
         * @param screenWidth The width of the screen in pixels.
         * @param screenHeight The height of the screen in pixels.
         * @details Initializes the editor with the given screen dimensions, setting up a default grid size and color.
         */
        Editor(unsigned int screenWidth, unsigned int screenHeight)
            : _screenWidth(screenWidth), _screenHeight(screenHeight),
              _currentColor(RED),
              pixelColors(screenWidth / _gridSize, std::vector<Color>(screenHeight / _gridSize, BLANK)) {
                updateCanvasOffset();
              }

        /**
         * @brief Destroys the Editor object.
         * @details Performs any necessary cleanup when the editor is destroyed.
         */
        ~Editor() {}

        /**
         * @brief Updates the state of the editor.
         * @details Handles input events, updates the canvas, and processes tool interactions.
         */
        void update();

        /**
         * @brief Draws the grid on the canvas.
         * @details Draws horizontal and vertical lines that represent the grid on the pixel art canvas.
         */
        void drawGrid();

        /**
         * @brief Draws the pixels on the canvas.
         * @details Renders the pixels on the canvas according to their stored colors in the `pixelColors` structure.
         */
        void drawPixels();

        /**
         * @brief Handles interaction with a pixel on the canvas.
         * @param x The x-coordinate of the mouse pointer (in screen space).
         * @param y The y-coordinate of the mouse pointer (in screen space).
         * @details Depending on the selected tool, this function modifies the corresponding pixel color or performs other actions.
         */
        void handlePixel(int x, int y);

        /**
         * @brief Provides the interface for customizing tools.
         * @details Includes UI elements for selecting the current drawing tool (Pen, Eraser, or Pipette)
         *          and adjusting the current color via sliders.
         */
        void customizationTools();

        /**
         * @brief Zooms in on the canvas.
         * @details Increases the zoom level, making the grid cells and pixels larger on the screen.
         */
        void zoomIn();

        /**
         * @brief Zooms out from the canvas.
         * @details Decreases the zoom level, making the grid cells and pixels smaller on the screen.
         */
        void zoomOut();

        /**
         * @brief Saves the current pixel art to a file.
         * @param filename The filename (including path) to save the pixel art image.
         * @details The image is saved as a PNG file with the provided filename.
         */
        void saveToFile(const std::string& filename);

        /**
         * @brief Loads pixel art from a file.
         * @param filename The filename (including path) of the image to load.
         * @details The pixel art is loaded from a PNG file, and the pixel data is stored in the `pixelColors` structure.
         */
        void loadFromFile(const std::string& filename);

        /**
         * @brief Gets the current tool selected.
         * @return The current tool in use (Pen, Eraser, or Pipette).
         */
        ToolType getTool() const { return _currentTool; }

        /**
         * @brief Gets the current color.
         * @return The color currently selected for drawing.
         */
        Color getCurrentColor() const { return _currentColor; }

        /**
         * @brief Gets the current zoom level.
         * @return The current zoom level of the canvas, between 0.5 and 2.0.
         */
        float getZoomLevel() const { return _zoomLevel; }

        /**
         * @brief Gets the grid size.
         * @return The current grid size in pixels.
         */
        int getGridSize() const { return _gridSize; }

        /**
         * @brief Sets the current tool.
         * @param tool The tool to be selected (Pen, Eraser, or Pipette).
         */
        void setTool(ToolType tool) { _currentTool = tool; }

        /**
         * @brief Sets the current color.
         * @param color The color to be selected for drawing.
         */
        void setCurrentColor(const Color& color) { _currentColor = color; }

        /**
         * @brief Sets the zoom level of the canvas.
         * @param zoomLevel The new zoom level (between 0.5 and 2.0).
         * @details Adjusts the grid size based on the zoom level and updates the canvas offset to maintain proper centering.
         */
        void setZoomLevel(float zoomLevel) {
            if (zoomLevel >= 0.5f && zoomLevel <= 2.0f) {
                _zoomLevel = zoomLevel;
                _gridSize = _originalGridSize * _zoomLevel;
                updateCanvasOffset();
            }
        }

    private:
        /**
         * @brief The width of the screen in pixels.
         * @details This value is set during the initialization of the Editor and is used to calculate the position of the grid and canvas.
         */
        unsigned int _screenWidth;

        /**
         * @brief The height of the screen in pixels.
         * @details This value is set during the initialization of the Editor and is used to calculate the position of the grid and canvas.
         */
        unsigned int _screenHeight;

        /**
         * @brief The size of the grid cells in pixels.
         * @details Default value is 20 pixels. This value is dynamically adjusted based on the zoom level.
         */
        int _gridSize = 20;

        /**
         * @brief The current drawing color.
         * @details The color used for drawing on the canvas. Default value is RED.
         */
        Color _currentColor;

        /**
         * @brief The current zoom level of the canvas.
         * @details Default value is 1.0 (no zoom). The zoom level can be adjusted between 0.5 and 2.0.
         */
        float _zoomLevel = 1.0f;

        /**
         * @brief The zoom step value.
         * @details This is the amount by which the zoom level is adjusted when zooming in or out.
         */
        const float _zoomStep = 0.1f;

        /**
         * @brief The original grid size before zoom.
         * @details This value is used as the baseline grid size, which is then adjusted based on the current zoom level.
         */
        const int _originalGridSize = 20;

        /**
         * @brief The horizontal offset for the canvas.
         * @details Used to center the canvas on the screen.
         */
        int _canvasOffsetX = 0;

        /**
         * @brief The vertical offset for the canvas.
         * @details Used to center the canvas on the screen.
         */
        int _canvasOffsetY = 0;

        /**
         * @brief The current tool in use.
         * @details Default value is Pen. This determines what action is performed when interacting with the canvas.
         */
        ToolType _currentTool = ToolType::Pen;

        /**
         * @brief A 2D vector representing the colors of the pixels on the canvas.
         * @details Each entry in the vector corresponds to a pixel, and its color is stored as a `Color` object.
         */
        std::vector<std::vector<Color>> pixelColors;

        /**
         * @brief Updates the canvas offset to center it on the screen.
         * @details The offset is recalculated based on the current grid size and screen dimensions to ensure proper alignment.
         */
        void updateCanvasOffset() {
            _canvasOffsetX = (_screenWidth - (pixelColors.size() * _gridSize)) / 2;
            _canvasOffsetY = (_screenHeight - (pixelColors[0].size() * _gridSize)) / 2;
        }
    };

}
