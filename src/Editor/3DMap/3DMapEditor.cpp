#include "iostream"

#include "3DMapEditor.hpp"
#include "../../../includes/Input/InputTypes.hpp"

MapEditor::MapEditor(Render::Camera &camera, Render::Window &window) : _window(window), _camera(camera), _grid(), _cubeHeight(1)
{
}

MapEditor::~MapEditor()
{
}

void MapEditor::initGrid()
{
    _grid.init();
}

void MapEditor::update(input::IHandlerBase &inputHandler)
{
    if (inputHandler.isReleased(input::Generic::SELECT1)) {
        Vector2D cursorPos = inputHandler.getCursorCoords();
        auto click = alignPosition(cursorPos).first;
        if (_objects3D.size() == 0 || click != Vector3D(0, 0, 0))
            addCube(click);
    }
    if (inputHandler.isReleased(input::Generic::SELECT2)) {
        Vector2D cursorPos = inputHandler.getCursorCoords();
        auto click = alignPosition(cursorPos).second;
        if (click != _objects3D.end() && !_objects3D.empty())
            removeCube(click);
    }
    if (inputHandler.isReleased(input::Generic::LEFT)) {
        _camera.rotateClock();
    }
    if (inputHandler.isReleased(input::Generic::RIGHT)) {
        _camera.rotateCounterclock();
    }
    if (inputHandler.isPressed(input::Generic::DOWN)) {
        saveMapBinary("game_project/assets/maps/game_map.dat");
        std::cout << "Save map" << std::endl;
    }
    if (inputHandler.isPressed(input::Generic::UP)) {
        loadMapBinary("game_project/assets/maps/game_map.dat");
        std::cout << "Load map" << std::endl;
    }
    // if (inputHandler.isReleased(input::Generic::INTERACT2)) {
    //     _currentCubeType.rotateModel(-PI / 2);
    // }
    // if (inputHandler.isReleased(input::Generic::INTERACT3)) {
    //     _currentCubeType.rotateModel(PI / 2);
    // }
}

void MapEditor::draw2DElements()
{
    for (auto i = _objects2D.begin(); i != _objects2D.end(); i++)
        i->draw();
}

void MapEditor::draw3DElements()
{
    _grid.draw();
    for (auto i = _objects3D.begin(); i != _objects3D.end(); i++)
        i->draw();
}

void MapEditor::changeCubeType(Asset3D newAsset)
{
    _currentCubeType = newAsset;
}

void MapEditor::addCube(Vector3D position)
{
    BasicObject3D newObject = BasicObject3D(_currentCubeType, position);
    newObject.resizeTo(_cubeHeight);
    _objects3D.push_back(newObject);
}

void MapEditor::removeCube(std::vector<BasicObject3D>::iterator toRemove)
{
    _objects3D.erase(toRemove);
}

std::pair<Vector3D, std::vector<BasicObject3D>::iterator> MapEditor::alignPosition(Vector2D cursorPos)
{
    Vector3D cameraPos = _camera.getPosition();
    Ray ray = GetMouseRay(cursorPos.convert(), _camera.getRaylibCam());

    RayCollision closestHit = { false, std::numeric_limits<float>::max(), { 0, 0, 0 }, { 0, 0, 0 } };
    auto closestObj = _objects3D.end();

    for (auto i = _objects3D.begin(); i != _objects3D.end(); i++) {
        RayCollision collision = GetRayCollisionBox(ray, i->getBox().convert());
        if (collision.hit && collision.distance < closestHit.distance) {
            closestHit = collision;
            closestObj = i;
        }
    }

    if (!closestHit.hit) {
        return {Vector3D(0, 0.5f, 0), _objects3D.end()};
    }

    Vector3D collisionPoint = closestHit.point;
    ObjectBox3D &modelBox = closestObj->getBox();
    Vector3D diff = collisionPoint - modelBox.position;
    Vector3D alignedPos = modelBox.position;

    if (diff.x == _cubeHeight)       alignedPos.x += _cubeHeight;
    else if (diff.x == 0) alignedPos.x -= _cubeHeight;
    else if (diff.z == _cubeHeight)  alignedPos.z += _cubeHeight;
    else if (diff.z == 0) alignedPos.z -= _cubeHeight;
    if (diff.y == _cubeHeight) alignedPos.y += _cubeHeight; // Y remains unchanged

    return {alignedPos, closestObj};
    return result;
}

void MapEditor::saveMapBinary(const std::string& filename)
{
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to open map file for saving!\n";
        return;
    }

    size_t objectCount = _objects3D.size();

    file.write(reinterpret_cast<const char*>(&objectCount), sizeof(objectCount));
    for (auto& obj : _objects3D) {
        Utilities::Vector3D position = obj.getPosition();
        file.write(reinterpret_cast<const char*>(&position.x), sizeof(position.x));
        file.write(reinterpret_cast<const char*>(&position.y), sizeof(position.y));
        file.write(reinterpret_cast<const char*>(&position.z), sizeof(position.z));
    }
    file.close();
    std::cout << "Map saved in binary format.\n";
}

void MapEditor::loadMapBinary(const std::string& filename)
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
        addCube(position);
    }
    file.close();
    std::cout << "Map loaded from binary file.\n";
}
