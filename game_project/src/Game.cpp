#include "Game.hpp"
#include <filesystem>
#include "Utilities/PathHelper.hpp"

Game::Game(std::shared_ptr<Render::Window> window, std::shared_ptr<Render::Camera> camera) : _cubeHeight(1)
{
    _window = window;
    _window->startWindow(Vector2D(SCREENWIDTH, SCREENHEIGHT));
    _camera = camera;

    std::filesystem::path exePath = Utilities::getExecutablePath();
    std::filesystem::path basePath = exePath.parent_path();

    std::string modelPath = (basePath / "ressources" / "elements" / "models" / "cube.obj").string();
    std::string mapPath = (exePath / "assets" / "maps" / "game_map.dat").string();
    std::string playerPath = (exePath / "assets" / "entities" / "shy_guy_red.png").string();

    _cubeType = Asset3D(modelPath);
    loadMap(mapPath);
    Vector3D playerPos(0, 0, 0);
    if (!_mapElements.empty()) {
        playerPos = _mapElements[0]->getBoxPosition();
    }
    playerPos.z -= 0.5f;
    _playerAsset = Asset2D(playerPath);
    _player = std::make_shared<objects::Character>(_playerAsset, playerPos, Vector2D(0, 0), Vector2D(32, 40));
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
    newCube->getBox3D().setScale(_cubeHeight);
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
    _player->draw({64, 32});
    for (auto i = _characters.begin(); i != _characters.end(); i++) {
        i->get()->draw();
    }
}

void Game::handleInput(input::IHandlerBase &inputHandler)
{
    const float gridStep = 0.1f;
    Vector3D playerPos = _player->getBoxPosition();

    if (inputHandler.isReleased(input::Generic::SELECT1)) {
        _camera->rotateClock();
        std::cout << "Rotate Camera" << std::endl;
    }
    if (inputHandler.isReleased(input::Generic::SELECT2)) {
        _camera->rotateCounterclock();
        std::cout << "Other Rotate Camera" << std::endl;
    }
    if (!inputHandler.isNotPressed(input::Generic::LEFT) || !inputHandler.isNotPressed(input::Generic::RIGHT) || !inputHandler.isNotPressed(input::Generic::UP) || !inputHandler.isNotPressed(input::Generic::DOWN)) {
        if (!inputHandler.isNotPressed(input::Generic::LEFT) && handleCollision({playerPos.x - gridStep, playerPos.y, playerPos.z})) {
            playerPos.x -= gridStep;
        }
        if (!inputHandler.isNotPressed(input::Generic::RIGHT) && handleCollision({playerPos.x + gridStep, playerPos.y, playerPos.z})) {
            playerPos.x += gridStep;
        }
        if (!inputHandler.isNotPressed(input::Generic::UP) && handleCollision({playerPos.x, playerPos.y, playerPos.z - gridStep})) {
            playerPos.z -= gridStep;
        }
        if (!inputHandler.isNotPressed(input::Generic::DOWN) && handleCollision({playerPos.x, playerPos.y, playerPos.z + gridStep})) {
            playerPos.z += gridStep;
        }
        _player->setBox3DPosition(playerPos);
        _player->setMoving(true);
    } else {
        _player->setMoving(false);
    }

}

bool Game::handleCollision(Utilities::Vector3D newPos)
{
    Utilities::Vector3D posTmp = {0.0f, 0.0f, 0.0f};
    bool thereIsACube = false;

    newPos = getEntitieBlockPos(newPos);
    for (auto i = _mapElements.begin(); i != _mapElements.end(); i++) {
        posTmp = i->get()->getBoxPosition();
        if ((newPos.x == posTmp.x && newPos.z == posTmp.z)) {
            if (newPos.y + 1 == posTmp.y || newPos.y - 1 == posTmp.y) {
                return false;
            }
        }
        if (newPos == posTmp)
            thereIsACube = true;
    }
    if (!thereIsACube)
        std::cout << "False" << std::endl;
    return thereIsACube;
}

Utilities::Vector3D Game::getEntitieBlockPos(Utilities::Vector3D pos)
{
    return {std::trunc(pos.x), pos.y, std::trunc(pos.z)};
}

void Game::update(input::IHandlerBase &inputHandler)
{
    handleInput(inputHandler);

    _player->updateAnimation();
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
    _window->startRender();
    _window->clearBackground(GRAY);
    drawVerticalGradient({ 0, 0, SCREENWIDTH, SCREENHEIGHT }, SKYBLUE, WHITE);
    _camera->start3D();
    draw3DElements();
    _camera->end3D();
    draw2DElements();
    _window->endRender();
}

void Game::loop(input::IHandlerBase &inputHandler)
{
    while (!_window->isWindowClosing()) {
        update(inputHandler);
        Render();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    _window->closeWindow();
}
