#include "Game.hpp"
#include <filesystem>
#include "Utilities/PathHelper.hpp"

Game::Game(Render::Window& window, Render::Camera& camera) : _window(window), _camera(camera)
{
    _cubeHeight = 1;
    _window.startWindow(Vector2D(SCREENWIDTH, SCREENHEIGHT));

    std::filesystem::path exePath = Utilities::getExecutablePath();
    std::filesystem::path basePath = exePath.parent_path();

    std::string modelPath = (basePath / "ressources" / "Block1.glb").string();
    std::string mapPath = (exePath / "assets" / "maps" / "game_map.dat").string();
    std::string playerPath = (exePath / "assets" / "entities" / "shy_guy_red.png").string();

    _cubeType.setFileName(modelPath);
    _cubeType.loadFile();
    loadMap(mapPath);
    _playerPos = {0, 0, 0};
    if (!_objects.empty()) {
        _playerPos = _objects[0].getPosition();
    }
    _playerPos.z -= 0.5f;
    _player.moveTo(_playerPos);
    _playerAsset.setFileName(playerPath);
    _playerAsset.loadFile();
    _player.setTexture(_playerAsset, 32, 40, 4);
}

Game::~Game()
{

}

void Game::addCube(Vector3D position)
{
    for (auto i = _objects.begin(); i != _objects.end(); i++) {
        if (i->getPosition() == position) {
            return;
        }
    }

    BasicObject newObject = BasicObject(_cubeType, position);
    std::cout << position.x << " " << position.y << " " << position.z << std::endl;
    newObject.resizeTo(_cubeHeight);
    _objects.push_back(newObject);
}

void Game::loadMap(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open map file for loading!\n";
        return;
    }
    std::string header;
    int count = 0;

    file >> header;
    if (header == "MAP") {
        file >> count;
        _objects.clear();
        for (int i = 0; i < count; ++i) {
            Vector3 position;
            file >> position.x >> position.y >> position.z;
            addCube(position);
        }
    }

    file >> header;
    if (header == "PLAYER") {
        Vector3D playerPos = {0, 0, 0};
        file >> playerPos.x >> playerPos.y;
        //addPlayer({playerPos.x, playerPos.y});
    }
    file.close();
    std::cout << "Map loaded.\n";
}

void Game::draw3DElements()
{
    for (auto i = _objects.begin(); i != _objects.end(); i++) {
        if (i->getAssetType() == AssetType::ASSET3D)
            i->draw();
    }
}

void Game::draw2DElements()
{
    _player.moveTo(_playerPos);
    _player.draw();
    for (auto i = _objects.begin(); i != _objects.end(); i++) {
        if (i->getAssetType() == AssetType::ASSET2D) {
            i->draw();
        }
    }
}

void Game::handleInput(input::IHandlerBase &inputHandler)
{
    bool moving = false;

    if (inputHandler.isReleased(input::Generic::SELECT1)) {
        _camera.rotateClock();
        std::cout << "Rotate Camera" << std::endl;
    }
    if (inputHandler.isReleased(input::Generic::SELECT2)) {
        _camera.rotateCounterclock();
        std::cout << "Other Rotate Camera" << std::endl;
    }
    if (inputHandler.isPressed(input::Generic::LEFT)) {
        _playerPos.x -= 0.1f;
        moving = true;
    }
    if (inputHandler.isPressed(input::Generic::RIGHT)) {
        _playerPos.x += 0.1f;
        moving = true;
    }
    if (inputHandler.isPressed(input::Generic::UP)) {
        _playerPos.z -= 0.1f;
        moving = true;
    }
    if (inputHandler.isPressed(input::Generic::DOWN)) {
        _playerPos.z += 0.1f;
        moving = true;
    }

    _player.setMoving(moving);
    _playerIsMoving = moving;
}


void Game::update(input::IHandlerBase &inputHandler)
{
    handleInput(inputHandler);

    if (_playerIsMoving) {
        _player.updateAnimation();
    }
}

void drawVerticalGradient(Rectangle rect, Color top, Color bottom) {
    for (int y = 0; y < rect.height; y++) {
        float alpha = (float)y / rect.height;
        Color c = {
            (unsigned char)(top.r + alpha * (bottom.r - top.r)),
            (unsigned char)(top.g + alpha * (bottom.g - top.g)),
            (unsigned char)(top.b + alpha * (bottom.b - top.b)),
            255
        };
        DrawRectangle(rect.x, rect.y + y, rect.width, 1, c);
    }
}

void Game::Render()
{
    _window.startRender();
    _window.clearBackground(GRAY);
    drawVerticalGradient({ 0, 0, SCREENWIDTH, SCREENHEIGHT }, SKYBLUE, WHITE);
    _camera.start3D();
    draw3DElements();
    _camera.end3D();
    draw2DElements();
    _window.endRender();
}

void Game::loop(input::IHandlerBase &inputHandler)
{
    while (!_window.isWindowClosing()) {
        update(inputHandler);
        Render();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    _window.closeWindow();
}
