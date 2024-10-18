#include "../Render/Window.hpp"
#include "../Render/Camera.hpp"
#include "../Assets/Asset3D.hpp"
#include "../Entity3D/StaticObject3D/StaticObject3D.hpp"
#include "../Utilities/Vector.hpp"

int main()
{
    //setup
    Render::Window window;
    Render::Camera camera;

    window.startwindow(Vector2D(1920, 1080));
    window.setFPS(60);

//    Asset3D blockAsset;
//    blockAsset.setFileName("../models/Block1.glb");
//
//    std::vector<std::shared_ptr<Entity3D::StaticObject3D>> staticObjects;
//    staticObjects.push_back(std::make_shared<Entity3D::StaticObject3D>(Entity3D::StaticObject3D(blockAsset, Vector3D(0, 0, 0))));

    while (!window.isWindowClosing()) {
        window.startRender();
        window.clearBackground();
        vcamera.start3D();
        //for (auto i = staticObjects.begin(); i != staticObjects.end(); i++)
        //    (*i)->draw();
        DrawGrid(10, 1.0f);
        camera.end3D();
        window.endRender();
    }
    window.closeWindow();
}