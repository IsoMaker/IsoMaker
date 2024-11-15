#include <iostream>
#include "raylib.h"
#include "Render/Camera.hpp"
#include "Render/Window.hpp"
#include "Object3D/BasicObject3D.hpp"

int main()
{
    Vector2D screen (800, 450);
    Render::Window window;
    Render::Camera camera;
    Asset3D asset;
    Object3D::BasicObject3D object;

    window.startWindow(screen);
    window.setFPS(60);

    asset.setFileName("./ressources/Block1.glb");
    asset.loadFile();
    object.setAsset(asset);

    while (!window.isWindowClosing()) {
        window.startRender();
            window.clearBackground(WHITE);
            camera.start3D();
                object.draw();
            camera.end3D();
        window.endRender();
    }
    CloseWindow();
    return 0;
}
