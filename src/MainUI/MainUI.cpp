#include "MainUI.hpp"
#include <thread>
#include <chrono>

MainUI::MainUI() : _3DMapEditor(_camera, _window) {
    _window.startWindow(Vector2D(SCREENWIDTH, SCREENHEIGHT));

    //temporary cube asset loading for the 3D map, to change after libraries are implemented
    _gameProjectName = "game_project";

    Asset3D cubeAsset;
    cubeAsset.setFileName("ressources/newBlock.glb");
    cubeAsset.loadFile();
    _3DMapEditor.changeCubeType(cubeAsset);

    Asset2D playerAsset;
    playerAsset.setFileName("ressources/shy_guy_red.png");
    playerAsset.loadFile();
    _3DMapEditor.changeSpriteType(playerAsset);
}

void MainUI::update(input::IHandlerBase &inputHandler) {
    Vector2D cursorPos = inputHandler.getCursorCoords();
    _3DMapEditor.update(inputHandler);
}

void MainUI::draw() {
    _window.startRender();
    _window.clearBackground(DARKBLUE);
    _camera.start3D();
    _3DMapEditor.draw3DElements();
    _camera.end3D();
    _3DMapEditor.draw2DElements();
    _window.endRender();
}

void MainUI::loop(input::IHandlerBase &mouseHandler) {
    _3DMapEditor.initGrid();
    while (!_window.isWindowClosing()) {
        update(mouseHandler);
        draw();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    _window.closeWindow();
}
