#include "Game.hpp"
#include <filesystem>
#include "Utilities/PathHelper.hpp"

Game::Game(Render::Window& window, Render::Camera& camera) : _window(window), _camera(camera)
{
    _cubeHeight = 1;
    _window.startWindow(Vector2D(1080, 960), "Random Game");

    std::filesystem::path exePath = Utilities::getExecutablePath();
    std::filesystem::path basePath = exePath.parent_path();

    std::string modelPath = (basePath / "ressources" / "Block1.glb").string();
    std::string mapPath   = (exePath / "assets" / "maps" / "game_map.dat").string();

    _cubeType.setFileName(modelPath);
    _cubeType.loadFile();
    loadMap(mapPath);
}

Game::~Game()
{

}

void Game::addCube(Vector3D position)
{
    BasicObject3D newObject = BasicObject3D(_cubeType, position);

    newObject.resizeTo(_cubeHeight);
    _objects3D.push_back(newObject);
}

void Game::loadMap(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open map file for loading!\n";
        return;
    }

    size_t objectCount;

    file.read(reinterpret_cast<char*>(&objectCount), sizeof(objectCount));
    _objects3D.clear();
    for (size_t i = 0; i < objectCount; ++i) {
        Utilities::Vector3D position;

        file.read(reinterpret_cast<char*>(&position.x), sizeof(position.x));
        file.read(reinterpret_cast<char*>(&position.y), sizeof(position.y));
        file.read(reinterpret_cast<char*>(&position.z), sizeof(position.z));
        std::cout << position.x << " " << position.y << " " << position.z;
        addCube(position);
    }
    file.close();
    std::cout << "Map loaded from binary file.\n";
}

void Game::draw3DElements()
{
    for (auto i = _objects3D.begin(); i != _objects3D.end(); i++)
        i->draw();
}

void Game::update(input::MouseHandler &mouseHandler, input::KeyboardHandler &keyboardHandler)
{
    // TBD
}

void Game::render()
{
    _window.startRender();
    _window.clearBackground(GRAY);
    _camera.start3D();
    draw3DElements();
    _camera.end3D();
    //draw2DElements();
    _window.endRender();
}

void Game::loop(input::MouseHandler &mouseHandler, input::KeyboardHandler &keyboardHandler)
{
    while (!_window.isWindowClosing()) {
        update(mouseHandler, keyboardHandler);
        render();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    _window.closeWindow();
}
