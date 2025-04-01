#pragma once

#include <vector>
#include <fstream>

#include "Render/Camera.hpp"
#include "Render/Window.hpp"
#include "Entities/BasicObject3D.hpp"
#include "Entities/BasicObject2D.hpp"
#include "Input/Mouse.hpp"
#include "Input/Keyboard.hpp"
#include "Utilities/Vector.hpp"

class Game {
    public:
        Game(Render::Window& window, Render::Camera& camera);
        ~Game();

        void addCube(Vector3D position);
        void loadMap(const std::string& filename);
        void draw3DElements();

        void update(input::MouseHandler &mouseHandler, input::KeyboardHandler &keyboardHandler);
        void render();
        void loop(input::MouseHandler &mouseHandler, input::KeyboardHandler &keyboardHandler);

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

