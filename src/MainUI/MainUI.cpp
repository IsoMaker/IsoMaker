#include "MainUI.hpp"
#include <thread>
#include <chrono>

MainUI::MainUI() : _3DMapEditor(_camera, _window) {
    _window.startWindow(Vector2D(SCREENWIDTH, SCREENHEIGHT));

    //temporary cube asset loading for the 3D map, to change after libraries are implemented
    Asset3D cubeAsset;
    cubeAsset.setFileName("ressources/Block1.glb");
    cubeAsset.loadFile();
    _3DMapEditor.changeCubeType(cubeAsset);
}

void MainUI::update(input::MouseHandler &mouseHandler) {
    Vector2D mousePos = mouseHandler.getMouseCoords();
    _3DMapEditor.update(mouseHandler);
}

void MainUI::draw() {
    _window.startRender();
    _window.clearBackground(GRAY);
    _camera.start3D();
    _3DMapEditor.draw3DElements();
    _camera.end3D();
    _3DMapEditor.draw2DElements();
    _window.endRender();
}

void MainUI::loop(input::MouseHandler &mouseHandler) {
    while (!_window.isWindowClosing()) {
        update(mouseHandler);
        draw();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    _window.closeWindow();
}