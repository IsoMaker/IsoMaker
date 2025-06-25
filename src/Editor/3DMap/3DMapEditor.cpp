#include "iostream"

#include "3DMapEditor.hpp"
#include "Input/InputTypes.hpp"

MapEditor::MapEditor(Render::Camera &camera, Render::Window &window) : _window(window), _camera(camera), _grid(), _cubeHeight(1), _placePlayer(false)
{
    _alignedPosition = Vector3D(0, 0.5f, 0);
    _closestObject = std::nullopt;
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
    Vector2D cursorPos = inputHandler.getCursorCoords();
    updateCursorInfo(cursorPos, _camera.getPosition());

    if (inputHandler.isReleased(input::Generic::SELECT1)) {
        if (_alignedPosition != Vector3D(0, 0, 0))
            addCube(_alignedPosition);
    }
    if (inputHandler.isReleased(input::Generic::SELECT2)) {
        if (!_objects3D.empty() && _closestObject.has_value())
            removeCube(_closestObject.value());
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
    // if (inputHandler.isReleased(input::Generic::INTERACT1)) {
    //     _drawWireframe = !_drawWireframe;
    // }
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
        i->get()->draw();
    }
}

void MapEditor::draw3DElements()
{
    _grid.draw();
    for (auto i = _objects3D.begin(); i != _objects3D.end(); i++)
        i->get()->draw();

    Vector3 size = (Vector3){ _cubeHeight, _cubeHeight, _cubeHeight };
    Color color = (Color){ 255, 255, 0, 128 }; // Yellow with 50% opacity
    DrawCubeV(_alignedPosition.convert(), size, color); // Draw solid cube
}

void MapEditor::changeCubeType(Asset3D newAsset)
{
    _currentCubeType = newAsset;
}

void MapEditor::changeSpriteType(Asset2D newAsset)
{
    _currentSpriteType = newAsset;
}

void MapEditor::addCube(Vector3D position)
{
    std::unique_ptr<MapElement> newObject = std::make_unique<MapElement>(_currentCubeType, position);
    newObject->getBox3D().setScale(_cubeHeight);
    std::cout << "Adding cube at position: " << position.x << ", " << position.y << ", " << position.z << std::endl;
    _objects3D.push_back(std::move(newObject));
}

void MapEditor::addPlayer(Vector2D position)
{
    std::unique_ptr<Character> newObject = std::make_unique<Character>(_currentSpriteType, Vector3D(position.x, position.y, 0.5), Vector3D(32, 40, 1));
    _objects2D.push_back(std::move(newObject));
}

void MapEditor::removeCube(std::vector<std::unique_ptr<MapElement>>::iterator toRemove)
{
    _objects3D.erase(toRemove);
}

void MapEditor::removePlayer(std::vector<std::unique_ptr<Character>>::iterator toRemove)
{
    _objects2D.erase(toRemove);
}

void MapEditor::findPositionFromHit(RayCollision &hit)
{
    Vector3D collisionPoint = hit.point;
    Vector3D modelPos = _closestObject.value()->get()->getBox3D().getPosition();
    Vector3D diff = collisionPoint - modelPos;

    if (diff.x == _cubeHeight) modelPos.x += _cubeHeight;
    else if (diff.x == 0) modelPos.x -= _cubeHeight;
    else if (diff.z == _cubeHeight)  modelPos.z += _cubeHeight;
    else if (diff.z == 0) modelPos.z -= _cubeHeight;
    else if (diff.y == _cubeHeight) modelPos.y += _cubeHeight;
    else return; // No valid alignment found

    _alignedPosition = modelPos;
}

void MapEditor::findPositionFromGrid(Ray &ray)
{
    auto gridCell = _grid.getCellFromRay(ray);
    if (gridCell.has_value()) {
        _alignedPosition = gridCell.value();
    } else {
        _alignedPosition = Vector3D(0, 0.5f, 0);
    }
}

void MapEditor::updateCursorInfo(Vector2D cursorPos, Vector3D cameraPos)
{
    Ray ray = GetMouseRay(cursorPos.convert(), _camera.getRaylibCam());
    RayCollision closestHit = { false, std::numeric_limits<float>::max(), { 0, 0, 0 }, { 0, 0, 0 } };

    _closestObject = std::nullopt;

    // check if we are over an object
    for (auto i = _objects3D.begin(); i != _objects3D.end(); i++) {
        RayCollision collision = GetRayCollisionBox(ray, i->get()->getBox3D().convert());
        if (collision.hit && collision.distance < closestHit.distance) {
            closestHit = collision;
            _closestObject = i;
        }
    }

    // if there was a hit, we adapt the position of the cube to be placed
    if (_closestObject.has_value()) {
        findPositionFromHit(closestHit);
    } else { // otherwise, we check if we are over the grid
        findPositionFromGrid(ray);
    }
}

void MapEditor::saveMapBinary(const std::string& filename)
{
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open map file for saving!\n";
        return;
    }

    Vector3D pos3D = Vector3D();
    Vector2D pos2D = Vector2D();

    file << "MAP\n";
    file << _objects3D.size() << "\n";
    for (auto& obj : _objects3D) {
        pos3D = obj.get()->getBox3D().getPosition();
        file << pos3D.x << " " << pos3D.y << " " << pos3D.z << "\n";
    }

    if (!_objects2D.empty()) {
        pos2D = _objects2D[0].get()->getBox2D().getPosition();
        file << "PLAYER\n";
        file << pos2D.x << " " << pos2D.y << "\n";
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
