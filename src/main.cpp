#define RAYGUI_IMPLEMENTATION

#include "MainUI/MainUI.hpp"

//int main()
//{
//    MainUI mainUI;
//    mainUI.loop();
//}

#include "./Render/Camera.hpp"
#include "./Render/Window.hpp"

#include "./Assets/Asset2D.hpp"
#include "./Object3D/BasicObject3D.hpp"

int main()
{
    Render::Window window;
    Render::Camera camera;

    window.startWindow(Vector2D(1600, 1200));
    Asset2D test2D;
    test2D.setFileName("ressources/PaintTab.png");
    test2D.loadFile();

    BasicObject3D object2D(test2D, Vector3D(0, 0, 0), Vector3D(10, 10, 10));

    while (!window.isWindowClosing()) {
        window.startRender();
        window.clearBackground(GRAY);
        camera.start3D();
        object2D.draw();
        DrawGrid(10, 1.0f);
        camera.end3D();
        window.endRender();
    }
    window.closeWindow();
}
