#pragma once

#include <vector>
#include <limits>
#include "raylib.h"
#include "rlgl.h"

#include <fstream>

#include "Render/Window.hpp"
#include "Render/Camera.hpp"
#include "Grid.hpp"

#include "Input/MouseKeyboard.hpp"

#include "Entities/MapElement.hpp"
#include "Entities/Character.hpp"

using namespace Utilities;
using namespace objects;

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
        void removeCube(std::vector<std::unique_ptr<MapElement>>::iterator);

        void changeSpriteType(Asset2D newAsset);
        void addPlayer(Vector2D);
        void removePlayer(std::vector<std::unique_ptr<Character>>::iterator toRemove);

        void saveMapBinary(const std::string& filename);
        void loadMapBinary(const std::string& filename);

        void gameCompilation(const std::string& gameProjectName);

    protected:
    private:

        void findPositionFromHit(RayCollision &hit);
        void findPositionFromGrid(Ray &ray);
        void updateCursorInfo(Vector2D cursorPos, Vector3D cameraPos);


        std::vector<std::unique_ptr<MapElement>> _objects3D;
        std::optional<std::vector<std::unique_ptr<MapElement>>::iterator> _closestObject;

        std::vector<std::unique_ptr<Character>> _objects2D;

        Asset3D _currentCubeType;
        Asset2D _currentSpriteType;

        Render::Window &_window;
        Render::Camera &_camera;
        map::MapGrid _grid;

        bool _placePlayer;

        float _cubeHeight;

        Vector3D _alignedPosition;
};
