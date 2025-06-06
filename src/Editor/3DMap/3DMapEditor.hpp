#pragma once

#include <vector>
#include <limits>
#include "raylib.h"
#include "rlgl.h"

#include <fstream>

#include "Entities/BasicObject.hpp"

#include "Render/Window.hpp"
#include "Render/Camera.hpp"
#include "Grid.hpp"

#include "Input/MouseKeyboard.hpp"

using namespace Utilities;

namespace asset {

    enum class Type
    {
       PLAYER,
       CUBE,
    };
}

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
        void removeCube(std::vector<BasicObject>::iterator);

        void changeSpriteType(Asset2D newAsset);
        void addPlayer(Vector2D);
        void removePlayer(std::vector<BasicObject>::iterator toRemove);

        void saveMapBinary(const std::string& filename);
        void loadMapBinary(const std::string& filename);

        void gameCompilation(const std::string& gameProjectName);

    protected:
    private:

        std::pair<Vector3D, std::optional<std::vector<BasicObject>::iterator>> alignPosition(Vector2D);
        void findPositionFromHit(RayCollision &hit);
        void findPositionFromGrid(Ray &ray);
        void updateCursorInfo(Vector2D cursorPos, Vector3D cameraPos);


        std::vector<BasicObject> _objects3D;
        std::vector<BasicObject> _objects2D;

        Asset3D _currentCubeType;
        Asset2D _currentSpireType;

        Render::Window &_window;
        Render::Camera &_camera;
        map::MapGrid _grid;

        bool _placePlayer;

        bool _drawWireframe = false;
        float _cubeHeight;

        Vector3D _alignedPosition;
        BasicObject _previewObject;
        std::optional<std::vector<BasicObject>::iterator> _closestObject;
};
