#include "Game.hpp"
#include <filesystem>
#include "Utilities/PathHelper.hpp"

Game::Game(Render::Window& window, Render::Camera& camera) : _window(window), _camera(camera)
{
    _cubeHeight = 1;
    _window.startWindow(Vector2D(SCREENWIDTH, SCREENHEIGHT));

    std::filesystem::path exePath = Utilities::getExecutablePath();
    std::filesystem::path basePath = exePath.parent_path();

    std::string modelPath = (basePath / "ressources" / "elements" / "models" / "cube.obj").string();
    std::string mapPath = (exePath / "assets" / "maps" / "game_map.dat").string();
    std::string playerPath = (exePath / "assets" / "entities" / "shy_guy_red.png").string();

    _cubeType.setFileName(modelPath);
    _cubeType.loadFile();
    loadMap(mapPath);
    Vector3D playerPos(0, 0, 0);
    if (!_mapElements.empty()) {
        playerPos = _mapElements[0].get()->getBoxPosition();
    }
    playerPos.y -= 0.5f;
    _playerAsset.setFileName(playerPath);
    _playerAsset.loadFile();
    _player = std::make_shared<objects::Character>(_playerAsset, playerPos, Vector2D(0, 0), Vector2D(32, 40));
    std::cout << "PLAYER POS DEFAULT: " << playerPos.x << " " << playerPos.y << " " << playerPos.z << std::endl;
}

Game::~Game()
{

}

void Game::addCube(Vector3D position)
{
    for (auto i = _mapElements.begin(); i != _mapElements.end(); i++) {
        if (i->get()->getBoxPosition() == position) {
            return;
        }
    }

    std::shared_ptr<objects::MapElement> newCube = std::make_shared<objects::MapElement>(_cubeType, position);
    std::cout << position.x << " " << position.y << " " << position.z << std::endl;
    newCube.get()->getBox3D().setScale(_cubeHeight);
    _mapElements.push_back(newCube);
}

void Game::loadMap(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open map file to load!\n";
        return;
    }
    std::string header;
    int count = 0;

    file >> header;
    if (header == "MAP") {
        file >> count;
        _mapElements.clear();
        for (int i = 0; i < count; ++i) {
            Vector3 position;
            file >> position.x >> position.y >> position.z;
            addCube(position);
        }
    }

    file >> header;
    if (header == "PLAYER") {
        // Vector3D playerPos = {0, 0, 0};
        Vector3D playerPos = _characters.begin()->get()->getBoxPosition() + Vector3D(_cubeHeight / 2, _cubeHeight, _cubeHeight / 2);
        file >> playerPos.x >> playerPos.y >> playerPos.z;
        //addPlayer({playerPos.x, playerPos.y});
    }
    file.close();
    std::cout << "Map loaded.\n";
}

void Game::draw3DElements()
{
    for (auto i = _mapElements.begin(); i != _mapElements.end(); i++)
        i->get()->draw();
}

void Game::draw2DElements()
{
    _player.get()->draw(_camera);
    for (auto i = _characters.begin(); i != _characters.end(); i++) {
        i->get()->draw(_camera);
    }
}

void Game::handleInput(input::IHandlerBase &inputHandler)
{
    bool moving = false;
    const float gridStep = 0.1f;
    Vector3D playerPos = _player.get()->getBox3D().getPosition();

    if (inputHandler.isReleased(input::Generic::SELECT1)) {
        _camera.rotateClock();
        std::cout << "Rotate Camera" << std::endl;
    }
    if (inputHandler.isReleased(input::Generic::SELECT2)) {
        _camera.rotateCounterclock();
        std::cout << "Other Rotate Camera" << std::endl;
    }
    if (!inputHandler.isNotPressed(input::Generic::LEFT) && handleCollision({playerPos.x - 0.1f, playerPos.y, playerPos.z})) {
        playerPos.x -= gridStep;
        moving = true;
    }
    if (!inputHandler.isNotPressed(input::Generic::RIGHT) && handleCollision({playerPos.x + 0.1f, playerPos.y, playerPos.z})) {
        playerPos.x += gridStep;
        moving = true;
    }
    if (!inputHandler.isNotPressed(input::Generic::UP) && handleCollision({playerPos.x, playerPos.y, playerPos.z - 0.1f})) {
        playerPos.z -= gridStep;
        moving = true;
    }
    if (!inputHandler.isNotPressed(input::Generic::DOWN) && handleCollision({playerPos.x, playerPos.y, playerPos.z + 0.1f})) {
        playerPos.z += gridStep;
        moving = true;
    }
    if (moving = true) {
        _player.get()->getBox3D().setPosition(playerPos);
    }

    _player.get()->setMoving(moving);
}

bool Game::handleCollision(Utilities::Vector3D newPos)
{
    Utilities::Vector3D posTmp = {0.0f, 0.0f, 0.0f};
    bool thereIsACube = false;

    std::cout << "FUTURE POS [BEFORE] ROUND: " << newPos.x << " " << newPos.y << " " << newPos.z << std::endl;
    newPos = getEntitieBlockPos(newPos);
    std::cout << "FUTURE POS [AFTER] ROUND: " << newPos.x << " " << newPos.y << " " << newPos.z << std::endl;
    for (auto i = _mapElements.begin(); i != _mapElements.end(); i++) {
        posTmp = i->get()->getBoxPosition();
        if ((newPos.x == posTmp.x && newPos.z == posTmp.z)) {
            std::cout << "Meme cube" << std::endl;
            if (newPos.y + 1 == posTmp.y || newPos.y - 1 == posTmp.y) {
                std::cout << "OMG COLLISION" << std::endl;
                return false;
            }
        }
        if (newPos == posTmp)
            thereIsACube = true;
    }
    return thereIsACube;
}

Utilities::Vector3D Game::getEntitieBlockPos(Utilities::Vector3D pos)
{
    return {std::trunc(pos.x), pos.y, std::trunc(pos.z)};
}

void Game::update(input::IHandlerBase &inputHandler)
{
    handleInput(inputHandler);

    if (_player.get()->isMoving()) {
        _player.get()->updateAnimation();
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
