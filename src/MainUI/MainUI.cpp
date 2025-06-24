#include "MainUI.hpp"
#include <thread>
#include <chrono>

MainUI::MainUI() 
    : _3DMapEditor(_camera, _window),
      _uiManager(SCREENWIDTH, SCREENHEIGHT) 
{
    _window.startWindow(Vector2D(SCREENWIDTH, SCREENHEIGHT));
    _uiManager.initialize();

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
    _uiManager.update(inputHandler);
}

void MainUI::draw() {
    _window.startRender();
    
    // Get the main view area from the UI manager
    Rectangle mainViewArea = _uiManager.getMainViewArea();
    
    // Draw 3D content in the main view area
    _window.clearBackground(UI::BACKGROUND);
    
    BeginScissorMode(mainViewArea.x, mainViewArea.y, mainViewArea.width, mainViewArea.height);
    _camera.start3D();
    _3DMapEditor.draw3DElements();
    _camera.end3D();
    EndScissorMode();
    
    // Draw 2D UI elements
    _3DMapEditor.draw2DElements();
    _uiManager.draw(_3DMapEditor);
    
    _window.endRender();
}

void MainUI::loop(input::IHandlerBase &inputHandler) {
    _3DMapEditor.initGrid();
    while (!_window.isWindowClosing()) {
        update(inputHandler);
        draw();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    _window.closeWindow();
}
