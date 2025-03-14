#pragma once

#include <vector>
#include <limits>

#include "../../Object3D/BasicObject3D.hpp"
#include "../../Object2D/BasicObject2D.hpp"

#include "../../Render/Camera.hpp"
#include "../../Render/Window.hpp"

#include "../../Input/Mouse.hpp"

using namespace Utilities;

class MapEditor {
    public:
        MapEditor(Render::Camera&, Render::Window&);
        ~MapEditor();

        void update(input::MouseHandler &mouseHandler);
        void draw2DElements();
        void draw3DElements();

        void changeCubeType(Asset3D);
        void addCube(Vector3D);
        void removeCube(Vector3D);

    protected:
        Vector3D alignPosition(Vector2D);

        std::vector<Object3D::BasicObject3D> _objects3D;
        std::vector<Object2D::BasicObject2D> _objects2D;

        Asset3D _currentCubeType;

        Render::Window &_window;
        Render::Camera &_camera;

        float _cubeHeight;

};