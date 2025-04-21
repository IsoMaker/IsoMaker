#pragma once

// #include "../Editor/2D/2DEditor.hpp"
#include "../Editor/3DMap/3DMapEditor.hpp"
#include "Entities/BasicObject2D.hpp"
#include "Input/MouseKeyboard.hpp"
#include "../UI/UIManager.hpp"
#include <iostream>

#define SCREENHEIGHT 1200
#define SCREENWIDTH 1600

// using namespace paint;

enum EditorType {
    PAINT,
    MAP,
};

class MainUI {
    public:
        MainUI();
        ~MainUI() {};

        void update(input::IHandlerBase &inputHandler);
        void draw();
        void loop(input::IHandlerBase &inputHandler);

    protected:
        Render::Camera _camera;
        Render::Window _window;
        MapEditor _3DMapEditor;
        std::string _gameProjectName;
        UI::UIManager _uiManager;
};
