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
        Game(std::shared_ptr<Render::Window> window, std::shared_ptr<Render::Camera> camera);
        ~Game();

        void addCube(Vector3D position);
        void addCharacter(Vector3D position);
        void addPlayer(Vector3D position);
        void loadMap(const std::string& filename);
        void draw3DElements();
        void draw2DElements();

        void update(input::IHandlerBase &mouseHandler);
        void Render();
        void loop(input::IHandlerBase &mouseHandler);
        void handleInput(input::IHandlerBase &mouseHandler);
        bool handleCollision(Utilities::Vector3D newPos);
        Utilities::Vector3D getEntitieBlockPos(Utilities::Vector3D pos);
        void changeCubeType(Asset3D asset);
        void changeSpriteType(Asset2D asset);

    protected:

    private:
        std::vector<std::shared_ptr<objects::MapElement>> _objects3D;
        std::vector<std::shared_ptr<objects::Character>> _objects2D;

        Asset3D _cubeType;
        Asset2D _playerAsset;

        std::shared_ptr<Render::Window> _window;             ///< Reference to the application window
        std::shared_ptr<Render::Camera> _camera;             ///< Reference to the 3D camera

        std::shared_ptr<objects::Character> _player;

        float _cubeHeight;
        bool _isJumping = false;
        float _jumpVelocity = 0.0f;
        float _gravity = -0.01f;
        float oldPosY = 0.0f;
};
