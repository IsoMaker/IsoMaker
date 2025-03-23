#include "MainUI.hpp"
#include <thread>
#include <chrono>

MainUI::MainUI() : _3DMapEditor(_camera, _window) {
    _window.startWindow(Vector2D(SCREENWIDTH, SCREENHEIGHT));

    //temporary cube asset loading for the 3D map, to change after libraries are implemented
    Asset3D cubeAsset;
    cubeAsset.setFileName("ressources/Block2.glb");
    cubeAsset.loadFile();
    _3DMapEditor.changeCubeType(cubeAsset);

    //temporary library setup
    _Library.add3DAsset("ressources/Block1.glb");
    _Library.add3DAsset("ressources/Block2.glb");
}

void MainUI::update(input::MouseHandler &mouseHandler) {
    Vector2D mousePos = mouseHandler.getMouseCoords();
    //_Library.get3DAssetFromClick(mouseHandler);
    _3DMapEditor.update(mouseHandler);
}

void MainUI::draw() {
    _window.startRender();
    _window.clearBackground(GRAY);
    _camera.start3D();
    _3DMapEditor.draw3DElements();
    _camera.end3D();
    _3DMapEditor.draw2DElements();
    _Library.draw();
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