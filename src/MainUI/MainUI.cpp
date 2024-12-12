#include "MainUI.hpp"

MainUI::MainUI() : _3DMapEditor(_camera, _window), _2DEditor(SCREENWIDTH, SCREENHEIGHT) {
    _window.startWindow(Vector2D(SCREENWIDTH, SCREENHEIGHT));
    _UIsize = ObjectBox2D(Vector2D(0, 0), Vector2D(SCREENWIDTH, 100), 1);
    _currentEditor = EditorType::PAINT;

    //temporary cube asset loading for the 3D map, to change after libraries are implemented
    Asset3D cubeAsset;
    cubeAsset.setFileName("ressources/Block1.glb");
    cubeAsset.loadFile();
    _3DMapEditor.changeCubeType(cubeAsset);

    Asset2D paintTab;
    paintTab.setFileName("ressources/PaintTab.png");
    paintTab.loadFile();
    Asset2D mapTab;
    mapTab.setFileName("ressources/MapTab.png");
    mapTab.loadFile();

    _tabs.push_back(std::make_pair(std::make_shared<BasicObject2D>(mapTab, Vector2D(0, 0)), MAP));
    _tabs.push_back(std::make_pair(std::make_shared<BasicObject2D>(paintTab, Vector2D(800, 0)), PAINT));
}

void MainUI::handleClick(Vector2D mousePos) {
    std::cout << "YEAH" << std::endl;
    for (auto i = _tabs.begin(); i != _tabs.end(); i++) {
        if (i->first->isInObject(mousePos))
            {_currentEditor = i->second; std::cout << "CHANGE TO " << i->second << std::endl;}
    }
}

void MainUI::update() {
    Vector2D mousePos = GetMousePosition();
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && _UIsize.isInBox(mousePos)) {
        handleClick(mousePos);
    } else if (_currentEditor == MAP) {
        _3DMapEditor.update();
    }
}

void MainUI::drawUI() {
    for (auto i = _tabs.begin(); i != _tabs.end(); i++)
        i->first->draw();
}

void MainUI::draw() {
    _window.startRender();
    _window.clearBackground(GRAY);
    _camera.start3D();
    if (_currentEditor == MAP)
        _3DMapEditor.draw3DElements();
    _camera.end3D();
    if (_currentEditor == MAP)
        _3DMapEditor.draw2DElements();
    if (_currentEditor == PAINT)
        _2DEditor.update();
    drawUI();
    _window.endRender();
}

void MainUI::loop() {
    while (!_window.isWindowClosing()) {
        update();
        draw();
    }
    _window.closeWindow();
}