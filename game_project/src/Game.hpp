#pragma once

#include <vector>
#include <fstream>

#include "Render/Camera.hpp"
#include "Render/Window.hpp"
#include "Entities/BasicObject3D.hpp"
#include "Entities/BasicObject2D.hpp"
#include "Input/Gamepad.hpp"
#include "Input/MouseKeyboard.hpp"
#include "Utilities/Vector.hpp"

#define SCREENHEIGHT 1200
#define SCREENWIDTH 1600

class Game {
    public:
        Game(Render::Window& window, Render::Camera& camera);
        ~Game();

        void addCube(Vector3D position);
        void loadMap(const std::string& filename);
        void draw3DElements();

        void update(input::IHandlerBase &mouseHandler);
        void render();
        void loop(input::IHandlerBase &mouseHandler);

    protected:

    private:
        std::vector<BasicObject3D> _objects3D;
        std::vector<BasicObject2D> _objects2D;

        Asset3D _cubeType;

        Render::Window &_window;
        Render::Camera &_camera;

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
