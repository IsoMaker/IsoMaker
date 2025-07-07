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

    std::string modelPath = (basePath / "ressources" / "Block1.obj").string();
    std::string mapPath = (exePath / "assets" / "maps" / "game_map.dat").string();
    std::string playerPath = (exePath / "assets" / "entities" / "shy_guy_red.png").string();

    _cubeType = Asset3D(modelPath);
    std::cout << "MODEL PATH " << modelPath << "\n";

    loadMap(mapPath);
}

Game::~Game()
{

}

void Game::changeCubeType(Asset3D newAsset)
{
    _cubeType = newAsset;
}

void Game::changeSpriteType(Asset2D newAsset)
{
    _playerAsset = newAsset;
}

void Game::addCube(Vector3D position)
{
    for (auto i = _objects3D.begin(); i != _objects3D.end(); i++) {
        if (i->get()->getBoxPosition() == position) {
            return;
        }
    }

    std::shared_ptr<objects::MapElement> newCube = std::make_shared<objects::MapElement>(_cubeType, position);
    std::cout << "ADD NEW CUBE POS: " << position.x << " " << position.y << " " << position.z << std::endl;
    newCube->getBox3D().setScale(_cubeHeight);
    _objects3D.push_back(newCube);
}

void Game::addCharacter(Vector3D position)
{
    for (auto i = _objects2D.begin(); i != _objects2D.end(); i++) {
        if (i->get()->getBoxPosition() == position) {
            return;
        }
    }
    for (auto i = _objects3D.begin(); i != _objects3D.end(); i++) {
        if (i->get()->getBoxPosition() == position) {
            return;
        }
    }

    std::shared_ptr<objects::Character> newCharacter = std::make_shared<objects::Character>(_playerAsset);
    newCharacter->setBox3DPosition(position);
    std::cout << "ADD NEW PLAYER POS: " << position << std::endl;
    std::cout << "[SCALE NEW PLAYER]: " << _playerAsset.getScale() << std::endl;
    newCharacter->setBox2DSize({_playerAsset.getWidth(), _playerAsset.getHeight()});
    newCharacter->setBox2DScale(_playerAsset.getScale());
    newCharacter->setTotalFrames(_playerAsset.getFramesCount());
    _objects2D.push_back(newCharacter);
}

void Game::addPlayer(Vector3D position)
{
    for (auto i = _objects2D.begin(); i != _objects2D.end(); i++) {
        if (i->get()->getBoxPosition() == position) {
            return;
        }
    }
    for (auto i = _objects3D.begin(); i != _objects3D.end(); i++) {
        if (i->get()->getBoxPosition() == position) {
            return;
        }
    }

    std::shared_ptr<objects::Character> newCharacter = std::make_shared<objects::Character>(_playerAsset);
    newCharacter->setBox3DPosition(position);
    std::cout << "ADD NEW PLAYER POS: " << position << std::endl;
    std::cout << "[SCALE NEW PLAYER]: " << _playerAsset.getScale() << std::endl;
    newCharacter->setBox2DSize({_playerAsset.getWidth(), _playerAsset.getHeight()});
    newCharacter->setBox2DScale(_playerAsset.getScale());
    newCharacter->setTotalFrames(_playerAsset.getFramesCount());
    _player = newCharacter;
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
        _objects3D.clear();

        for (int i = 0; i < count; ++i) {
            Vector3 position;
            std::string filePath;
            float scale;

            file >> position.x >> position.y >> position.z >> filePath >> scale;
            std::cout << "FILENAME " << filePath << "\n";
            std::cout << "POSITION: " << position << "\n";
            Asset3D tmpAsset(filePath);
            tmpAsset.loadFile();
            tmpAsset.setScale(scale);
            changeCubeType(tmpAsset);
            addCube(position);
        }
    }

    int count2 = 0;
    file >> header;
    if (header == "PLAYER") {
        file >> count2;
        _objects2D.clear();
        Vector3D position = {0, 0, 0};
        Vector2 size;
        std::string filePath;
        float scale;
        int frames;
        for (int i = 0; i < count2; ++i) {
            file >> position.x >> position.y >> position.z >> filePath >>
                size.x >> size.y >> scale >> frames;
            std::cout << "FILENAME " << filePath << "\n";
            std::cout << "POSITION: " << position << "\n";
            std::cout << "SIZE: " << size << "\n";
            std::cout << "SCALE: " << scale << "\n";
            std::cout << "Frames: " << frames << "\n";
            Asset2D tmpAsset(filePath);
            tmpAsset.loadFile();
            tmpAsset.setScale(scale);
            tmpAsset.setWidth(size.x);
            tmpAsset.setHeight(size.y);
            tmpAsset.setFramesCount(frames);
            changeSpriteType(tmpAsset);
            if (i == 0)
                addPlayer(position);
            else
                addCharacter(position);
            std::cout << "ADD NEW PLAYER : " << i << "\n";
        }
    }
    file.close();
    std::cout << "Map loaded.\n";
}

void Game::draw3DElements()
{
    for (auto i = _objects3D.begin(); i != _objects3D.end(); i++)
        i->get()->draw();
}

void Game::draw2DElements()
{
    _player->draw(_player->getBox2D().getRectangle(), _camera);
    for (auto i = _objects2D.begin(); i != _objects2D.end(); i++) {
        i->get()->draw(i->get()->getBox2D().getRectangle(), _camera);
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
    if (!inputHandler.isNotPressed(input::Generic::ATTACK) && !_isJumping) {
        std::cout << "JUMP !\n";
        _isJumping = true;
        _jumpVelocity = 0.15f;
        oldPosY = playerPos.y;
    }
}

bool Game::handleCollision(Utilities::Vector3D newPos)
{
    bool blocBelow = false;
    int blocWidth = 1;
    int blocHeight = 1;

    for (auto i = _objects3D.begin(); i != _objects3D.end(); i++) {
        Utilities::Vector3D posTmp = i->get()->getBoxPosition();

        if (newPos.x >= posTmp.x && newPos.x <= posTmp.x + 1 &&
            newPos.z >= posTmp.z && newPos.z <= posTmp.z + 1 &&
            newPos.y >= posTmp.y && newPos.y < posTmp.y + 1) {
            return false;
        }
        if (newPos.x >= posTmp.x && newPos.x <= posTmp.x + 1 &&
            newPos.z >= posTmp.z && newPos.z <= posTmp.z + 1) {
                if (std::abs((posTmp.y + 1.0f) - newPos.y) < 0.05f) {
                    blocBelow = true;
                }
        }
    }
    return blocBelow;
}

Utilities::Vector3D Game::getEntitieBlockPos(Utilities::Vector3D pos)
{
    return {std::trunc(pos.x), pos.y, std::trunc(pos.z)};
}

void Game::update(input::IHandlerBase &inputHandler)
{
    handleInput(inputHandler);

    if (_isJumping) {
        Vector3D playerPos = _player->getBoxPosition();
        _jumpVelocity += _gravity;
        playerPos.y += _jumpVelocity;

        if (_jumpVelocity <= 0 && handleCollision({playerPos.x, playerPos.y - 0.01f, playerPos.z})) {
            _isJumping = false;
            _jumpVelocity = 0;
            playerPos.y -= 0.01f;
        } else if (_jumpVelocity <= 0) {
            _isJumping = false;
            _jumpVelocity = 0;
            playerPos.y = oldPosY;
        }

        _player->setBox3DPosition(playerPos);
    }

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
