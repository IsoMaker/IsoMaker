#include "iostream"

#include "3DMapEditor.hpp"
#include "Input/InputTypes.hpp"

MapEditor::MapEditor(Render::Camera &camera, Render::Window &window) : _window(window), _camera(camera), _grid(), _cubeHeight(1), _placePlayer(false)
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
        if (_objects3D.size() == 0 || click != Vector3D(0, 0, 0)) {
            if (!_placePlayer) {
                std::cout << "Place Cube !!" << std::endl;
                addCube(click);
            } else if (_placePlayer) {
                std::cout << "Place Player !!" << std::endl;
                addPlayer({click.x, click.y});
            }
        }
    }
    if (inputHandler.isReleased(input::Generic::SELECT2)) {
        Vector2D cursorPos = inputHandler.getCursorCoords();
        auto click = alignPosition(cursorPos).second;
        if (click != _objects3D.end() && !_objects3D.empty()) {
            if (!_placePlayer)
                removeCube(click);
        }
    }
    if (inputHandler.isReleased(input::Generic::LEFT)) {
        _camera.rotateClock();
        std::cout << "Rotate Camera" << std::endl;
    }
    if (inputHandler.isReleased(input::Generic::RIGHT)) {
        _camera.rotateCounterclock();
        std::cout << "Other Rotate Camera" << std::endl;
    }
    if (inputHandler.isPressed(input::Generic::DOWN)) {
        saveMapBinary("game_project/assets/maps/game_map.dat");
        std::cout << "Save map" << std::endl;
    }
    if (inputHandler.isPressed(input::Generic::UP)) {
        loadMapBinary("game_project/assets/maps/game_map.dat");
        std::cout << "Load map" << std::endl;
    }
    if (inputHandler.isPressed(input::Generic::ATTACK)) {
        gameCompilation("game_project");
        std::cout << "Game compilation" << std::endl;
    }
    if (inputHandler.isPressed(input::Generic::INVENTORY)) {
        std::cout << "Switch mode" << std::endl;
        if (_placePlayer) {
            _placePlayer = false;
        } else
            _placePlayer = true;
    }
    if (inputHandler.isReleased(input::Generic::INTERACT1)) {
        _drawWireframe = !_drawWireframe;
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
    for (auto i = _objects2D.begin(); i != _objects2D.end(); i++) {
        i->draw();
    }
}

void MapEditor::draw3DElements()
{
    _grid.draw();
    if (_drawWireframe) {
        for (auto i = _objects3D.begin(); i != _objects3D.end(); i++) {
            i->drawWireframe();
        }
    } else {
        for (auto i = _objects3D.begin(); i != _objects3D.end(); i++)
            i->draw();
    }
}

void MapEditor::changeCubeType(Asset3D newAsset)
{
    _currentCubeType = newAsset;
}

void MapEditor::changeSpriteType(Asset2D newAsset)
{
    _currentSpireType = newAsset;
}

void MapEditor::addCube(Vector3D position)
{
    BasicObject newObject = BasicObject(_currentCubeType, position);
    newObject.resizeTo(_cubeHeight);
    _objects3D.push_back(newObject);
}

void MapEditor::addPlayer(Vector2D position)
{
    BasicObject newObject = BasicObject(_currentSpireType, {position.x, position.y, 0.5}, Vector3D(32, 40, 1));
    _objects2D.push_back(newObject);
}

void MapEditor::removeCube(std::vector<BasicObject>::iterator toRemove)
{
    _objects3D.erase(toRemove);
}

void MapEditor::removePlayer(std::vector<BasicObject>::iterator toRemove)
{
    _objects2D.erase(toRemove);
}

std::pair<Vector3D, std::vector<BasicObject>::iterator> MapEditor::alignPosition(Vector2D cursorPos)
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
        auto gridPosOpt = _grid.getCellFromRay(ray);
        if (gridPosOpt.first) {
            return { gridPosOpt.second, _objects3D.end() };
        } else {
            return { Vector3D(0, 0, 0), _objects3D.end() };
        }
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
}

void MapEditor::saveMapBinary(const std::string& filename)
{
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open map file for saving!\n";
        return;
    }

    file << "MAP\n";
    file << _objects3D.size() << "\n";
    for (auto& obj : _objects3D) {
        file << obj.getPosition().x << " " << obj.getPosition().y << " " << obj.getPosition().z << "\n";
    }

    if (!_objects2D.empty()) {
        file << "PLAYER\n";
        file << _objects2D[0].getPosition().x << " " << _objects2D[0].getPosition().y << "\n";
    }
    file.close();
    std::cout << "Map saved.\n";
}


void MapEditor::loadMapBinary(const std::string& filename)
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
            file >> position.x >> position.y >> position.z;
            addCube(position);
        }
    }

    file >> header;
    if (header == "PLAYER") {
        Vector3D playerPos = {0, 0, 0};
        file >> playerPos.x >> playerPos.y;
        addPlayer({playerPos.x, playerPos.y});
    }
    file.close();
    std::cout << "Map loaded.\n";
}

void MapEditor::gameCompilation(const std::string& gameProjectName)
{
    std::string script = gameProjectName + "/install-linux.sh";

    system(script.c_str());
    system("./game_project/GenericGame");
}
