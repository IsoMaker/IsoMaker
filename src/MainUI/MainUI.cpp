#include "MainUI.hpp"
#include <thread>
#include <chrono>

MainUI::MainUI(std::shared_ptr<Render::Camera> camera, std::shared_ptr<Render::Window> window) : _uiManager(SCREENWIDTH, SCREENHEIGHT) 
{
    _camera = camera;
    _window = window;
    _window.get()->startWindow(Vector2D(SCREENWIDTH, SCREENHEIGHT));
    
    _3DMapEditor.init(window, camera);
    _uiManager.initialize();

    _gameProjectName = "game_project";
    
    _loader = std::make_shared<AssetLoader>();
    _3DMapEditor.setLoader(_loader);
    _uiManager.setLoader(_loader);
    //temporary cube asset loading for the 3D map, to change after libraries are implemented
    initMapEditorAssets();
}

MainUI::~MainUI()
{
}

void MainUI::initMapEditorAssets()
{
    Color myColor = LIGHTGRAY;
    Image colorImage = GenImageColor(1, 1, myColor);
    Texture2D colorTexture = LoadTextureFromImage(colorImage);
    UnloadImage(colorImage);
    Asset2D textureAsset(colorTexture);
    // Asset2D textureAsset("ressources/textures/cube_texture.png")
    _3DMapEditor.changeTextureType(textureAsset);

    Asset3D cubeAsset("ressources/elements/models/cube.obj");
    if (textureAsset.isLoaded())
        cubeAsset.setModelTexture(textureAsset.getTexture());
    _3DMapEditor.changeCubeType(cubeAsset);

    Asset2D playerAsset;
    playerAsset.setFileName("ressources/shy_guy_red.png");
    playerAsset.loadFile();
    _3DMapEditor.changeSpriteType(playerAsset);
}

void MainUI::update(input::IHandlerBase &inputHandler) {
    Vector2D cursorPos = inputHandler.getCursorCoords();

    _loader->updateAssets("ressources/loadedAssets");
    _3DMapEditor.setLoader(_loader);
    _uiManager.setLoader(_loader);
    _3DMapEditor.update(inputHandler);
    _uiManager.update(inputHandler);
}

void MainUI::draw() {
    _window.get()->startRender();

    // Get the main view area from the UI manager
    Rectangle mainViewArea = _uiManager.getMainViewArea();

    // Draw 3D content in the main view area
    _window.get()->clearBackground(UI::BACKGROUND);

    // Draw Map Editor 3D and 2D elements
    _3DMapEditor.draw(mainViewArea, _camera);
    // Draw Map Editor UI elements
    _uiManager.draw(_3DMapEditor);
    
    _window.get()->endRender();
}

void MainUI::loop(input::IHandlerBase &inputHandler) {
    while (!_window.get()->isWindowClosing()) {
        update(inputHandler);
        draw();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    _window.get()->closeWindow();
}
