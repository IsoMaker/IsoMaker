#pragma once

#include "../../Object3D/BasicObject3D.hpp"
#include "../../Object2D/BasicObject2D.hpp"

#include "../../Render/Camera.hpp"
#include "../../Render/Window.hpp"

using namespace Utilities;

class MapEditor {
    public:
        MapEditor(Render::Camera&, Render::Window&);
        ~MapEditor();

        void update();
        void draw2DElements();
        void draw3DElements();

        void changeCubeType(Asset3D);
        void addCube(Vector3D);
        void removeCube(std::vector<BasicObject3D>::iterator);

    protected:
        std::pair<Vector3D, std::vector<BasicObject3D>::iterator> alignPosition(Vector2D);

        std::vector<BasicObject3D> _objects3D;
        std::vector<BasicObject2D> _objects2D;

        Asset3D _currentCubeType;

        Render::Window &_window;
        Render::Camera &_camera;

        float _cubeHeight;

};