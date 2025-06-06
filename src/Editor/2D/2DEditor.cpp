#include "2DEditor.hpp"

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
        int x = _canvasOffsetX + i * _cellAmount;
        DrawLine(x, _canvasOffsetY, x, _canvasOffsetY + rows * _cellAmount, LIGHTGRAY);
    }
    for (int j = 0; j <= rows; j++) {
        int y = _canvasOffsetY + j * _cellAmount;
        DrawLine(_canvasOffsetX, y, _canvasOffsetX + cols * _cellAmount, y, LIGHTGRAY);
    }
}

void paint::Editor::drawPixels()
{
    for (int i = 0; i < pixelColors.size(); i++) {
        for (int j = 0; j < pixelColors[i].size(); j++) {
            int x = _canvasOffsetX + i * _cellAmount;
            int y = _canvasOffsetY + j * _cellAmount;
            DrawRectangle(x, y, _cellAmount, _cellAmount, pixelColors[i][j]);
        }
    }
}

void paint::Editor::handlePixel(int x, int y)
{
    int col = (x - _canvasOffsetX) / _cellAmount;
    int row = (y - _canvasOffsetY) / _cellAmount;

    if (col >= 0 && col < pixelColors.size() && row >= 0 && row < pixelColors[0].size()) {
        switch (getTool()) {
            case ToolType::Pen:
                pixelColors[col][row] = getCurrentColor();
                break;
            case ToolType::Eraser:
                pixelColors[col][row] = BLANK;
                break;
            case ToolType::Pipette:
                setCurrentColor(pixelColors[col][row]);
                break;
        }
    }
}

void paint::Editor::customizationTools()
{
    float red = _currentColor.r;
    float green = _currentColor.g;
    float blue = _currentColor.b;

    DrawText("Custom Color Picker", 20, 480, 10, DARKGRAY);
    GuiSliderBar((Rectangle){ 20, 490, 200, 20 }, "0", "255", &red, 0, 255);
    GuiSliderBar((Rectangle){ 20, 520, 200, 20 }, "0", "255", &green, 0, 255);
    GuiSliderBar((Rectangle){ 20, 550, 200, 20 }, "0", "255", &blue, 0, 255);

    _currentColor = { (unsigned char)red, (unsigned char)green, (unsigned char)blue, 255 };
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

    if (GuiButton((Rectangle){ 400, 490, 60, 30 }, "Save")) {
        saveToFile("pixel_art.png");
    }
    if (GuiButton((Rectangle){ 400, 520, 60, 30 }, "Load")) {
        loadFromFile("pixel_art.png");
    }
}

void paint::Editor::zoomIn()
{
    setZoomLevel(getZoomLevel() + _zoomStep);
}

void paint::Editor::zoomOut()
{
    setZoomLevel(getZoomLevel() - _zoomStep);
}

void paint::Editor::saveToFile(const std::string& filename)
{
    int cols = pixelColors.size();
    int rows = pixelColors[0].size();
    Image image = GenImageColor(cols, rows, BLANK);

    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            ImageDrawPixel(&image, x, y, pixelColors[x][y]);
        }
    }
    ExportImage(image, filename.c_str());
    UnloadImage(image);
    std::cout << "Saved PNG to: " << filename << std::endl;
}

void paint::Editor::loadFromFile(const std::string& filename)
{
    Image image = LoadImage(filename.c_str());

    if (image.data == nullptr) {
        std::cerr << "Failed to load PNG file: " << filename << std::endl;
        return;
    }
    pixelColors.resize(image.width, std::vector<Color>(image.height, BLANK));
    for (int y = 0; y < image.height; y++) {
        for (int x = 0; x < image.width; x++) {
            Color color = GetImageColor(image, x, y);
            pixelColors[x][y] = color;
        }
    }
    updateCanvasOffset();
    UnloadImage(image);
    std::cout << "Loaded PNG from: " << filename << std::endl;
}
