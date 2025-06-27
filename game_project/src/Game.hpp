#pragma once

#include <vector>
#include <fstream>
#include <cmath>

// Library
#include "Render/Camera.hpp"
#include "Render/Window.hpp"
#include "Utilities/Vector.hpp"

#include "Entities/MapElement.hpp"
#include "Entities/Character.hpp"

#include "Input/Gamepad.hpp"
#include "Input/MouseKeyboard.hpp"

#define SCREENHEIGHT 1200
#define SCREENWIDTH 1600

class Game {
    public:
        Game(Render::Window& window, Render::Camera& camera);
        ~Game();

        void addCube(Vector3D position);
        void loadMap(const std::string& filename);
        void draw3DElements();
        void draw2DElements();

        void update(input::IHandlerBase &mouseHandler);
        void Render();
        void loop(input::IHandlerBase &mouseHandler);
        void handleInput(input::IHandlerBase &mouseHandler);
        bool handleCollision(Utilities::Vector3D newPos);
        Utilities::Vector3D getEntitieBlockPos(Utilities::Vector3D pos);

    protected:

    private:
        std::vector<std::shared_ptr<objects::MapElement>> _mapElements;
        std::vector<std::shared_ptr<objects::Character>> _characters;

        Asset3D _cubeType;
        Asset2D _playerAsset;

        Render::Window &_window;
        Render::Camera &_camera;

        std::shared_ptr<objects::Character> _player;

        float _cubeHeight;
};
