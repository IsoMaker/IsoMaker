#pragma once

#include "../Editor/2D/2DEditor.hpp"
#include "../Editor/3DMap/3DMapEditor.hpp"
#include "../Object2D/BasicObject2D.hpp"
#include "../Input/Mouse.hpp"
#include <iostream>

#define SCREENHEIGHT 1200
#define SCREENWIDTH 1600

using namespace paint;

enum EditorType {
    PAINT,
    MAP,
};

class MainUI {
    public:
        MainUI();
        ~MainUI() {};

        void update(input::MouseHandler &mouseHandler);
        void draw();
        void loop(input::MouseHandler &mouseHandler);

    protected:
        Render::Camera _camera;
        Render::Window _window;
        MapEditor _3DMapEditor;
};