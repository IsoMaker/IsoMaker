#pragma once

#include "../Editor/2D/2DEditor.hpp"
#include "../Editor/3DMap/3DMapEditor.hpp"
#include "../Object2D/BasicObject2D.hpp"
#include "../Input/Mouse.hpp"
#include "../Library/Library.hpp"
#include <iostream>

using namespace paint;
using namespace Object2D;

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
        Library _Library;
};