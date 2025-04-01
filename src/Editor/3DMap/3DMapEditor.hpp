#pragma once

#include <vector>
#include <limits>
#include "raylib.h"
#include "rlgl.h"

#include <fstream>

#include "../../Object3D/BasicObject3D.hpp"
#include "../../Object2D/BasicObject2D.hpp"

#include "../../Render/Window.hpp"
#include "../../Render/Camera.hpp"
#include "Grid.hpp"

#include "../../Input/MouseKeyboard.hpp"

using namespace Utilities;

class MapEditor {
    public:
        MapEditor(Render::Camera&, Render::Window&);
        ~MapEditor();

        void initGrid();

        void update(input::IHandlerBase &inputHandler);

        void draw2DElements();
        void draw3DElements();

        void changeCubeType(Asset3D);
        void addCube(Vector3D);
        void removeCube(std::vector<BasicObject3D>::iterator);

        void saveMapBinary(const std::string& filename);
        void loadMapBinary(const std::string& filename);

        void gameCompilation(const std::string& gameProjectName);

    protected:
    private:
        std::pair<Vector3D, std::vector<BasicObject3D>::iterator> alignPosition(Vector2D);

        std::vector<BasicObject3D> _objects3D;
        std::vector<BasicObject2D> _objects2D;

        Asset3D _currentCubeType;

        Render::Window &_window;
        Render::Camera &_camera;
        MapGrid _grid;

        float _cubeHeight;
};
