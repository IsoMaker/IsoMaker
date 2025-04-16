#pragma once

#include <vector>
#include <fstream>

// Library
#include "Render/Camera.hpp"
#include "Render/Window.hpp"
#include "Entities/BasicObject3D.hpp"
#include "Entities/BasicObject2D.hpp"
#include "Input/Gamepad.hpp"
#include "Input/MouseKeyboard.hpp"
#include "Utilities/Vector.hpp"

// Generic Game
#include "Character.hpp"

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
        void render();
        void loop(input::IHandlerBase &mouseHandler);
        void handleInput(input::IHandlerBase &mouseHandler);

    protected:

    private:
        std::vector<BasicObject3D> _objects3D;
        std::vector<BasicObject2D> _objects2D;

        Asset3D _cubeType;
        Asset2D _playerAsset;

        Render::Window &_window;
        Render::Camera &_camera;

        Character _player;

        float _cubeHeight;


};

/*
    - Window
    - Camera ? Entities ??
    - Map3D
    - Entities
    - Event
        - Audio
            - SFX
            - Music
        - Log
        - VFX
*/
