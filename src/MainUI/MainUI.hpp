#pragma once

#include "../Editor/2D/2DEditor.hpp"
#include "../Editor/3DMap/3DMapEditor.hpp"
#include "../Object2D/BasicObject2D.hpp"

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

        void handleClick(Vector2D mousePos);
        void update();
        void draw();
        void drawUI();
        void loop();

    protected:
        Render::Camera _camera;
        Render::Window _window;
        Editor _2DEditor;
        MapEditor _3DMapEditor;
        EditorType _currentEditor;
        ObjectBox2D _UIsize;
        std::vector<std::pair<std::shared_ptr<BasicObject2D>, EditorType>> _tabs;
};