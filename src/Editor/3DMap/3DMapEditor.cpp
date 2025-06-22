#include "iostream"
#include <filesystem>

#include "3DMapEditor.hpp"
#include "Input/InputTypes.hpp"

MapEditor::MapEditor(Render::Camera &camera, Render::Window &window) : _window(window), _camera(camera), _grid(), _cubeHeight(1), _placePlayer(false)
{
    _alignedPosition = Vector3D(0, 0.5f, 0);
    _closestObject = std::nullopt;
    _drawWireframe = false;
    
    setupEventHandlers();
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
        if (_currentTool == 4 && _alignedPosition != Vector3D(0, 0, 0)) { // CUBE tool
            addCube(_alignedPosition);
            UI::Events::objectCreated(_alignedPosition.convert());
            notifySceneChanged();
        }
    }
    if (inputHandler.isReleased(input::Generic::SELECT2)) {
        if (!_objects3D.empty() && _closestObject.has_value()) {
            removeCube(_closestObject.value());
            notifySceneChanged();
        }
    }
    if (inputHandler.isReleased(input::Generic::LEFT)) {
        _camera.rotateClock();
        UI::Events::cameraMove(_camera.getPosition().convert());
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
        _gridVisible = !_gridVisible;
        UI::Events::gridToggled(_gridVisible);
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
    _currentSpireType = newAsset;
}

void MapEditor::addCube(Vector3D position)
{
    BasicEntity newObject = BasicEntity(_currentCubeType, position);
    newObject.resizeTo(_cubeHeight);
    std::cout << "Adding cube at position: " << position.x << ", " << position.y << ", " << position.z << std::endl;
    _objects3D.push_back(newObject);
}

void MapEditor::addPlayer(Vector2D position)
{
    BasicEntity newObject = BasicEntity(_currentSpireType, {position.x, position.y, 0.5}, Vector3D(32, 40, 1));
    _objects2D.push_back(newObject);
}

void MapEditor::removeCube(std::vector<BasicEntity>::iterator toRemove)
{
    _objects3D.erase(toRemove);
}

void MapEditor::removePlayer(std::vector<BasicEntity>::iterator toRemove)
{
    _objects2D.erase(toRemove);
}

void MapEditor::findPositionFromHit(RayCollision &hit)
{
    Vector3D collisionPoint = hit.point;
    Vector3D modelPos = _closestObject.value()->getPosition();
    Vector3D diff = collisionPoint - modelBox.position;
    Vector3D alignedPos = modelBox.position;

    if (diff.x == _cubeHeight) alignedPos.x += _cubeHeight;
    else if (diff.x == 0) alignedPos.x -= _cubeHeight;
    else if (diff.z == _cubeHeight)  alignedPos.z += _cubeHeight;
    else if (diff.z == 0) alignedPos.z -= _cubeHeight;
    else if (diff.y == _cubeHeight) alignedPos.y += _cubeHeight;
    else return; // No valid alignment found

    _alignedPosition = alignedPos;
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
        RayCollision collision = GetRayCollisionBox(ray, i->getBox().convert());
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
    std::filesystem::path filepath(filename);
    std::filesystem::create_directories(filepath.parent_path());

    std::ofstream file(filename, std::ios::out);
    if (!file.is_open()) {
        std::cerr << "Failed to open or create map file for saving: " << filename << "\n";
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
    std::cout << "Map saved to: " << filename << "\n";
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

void MapEditor::setupEventHandlers()
{
    // Subscribe to UI events
    UI::g_eventDispatcher.subscribe(UI::EditorEventType::TOOL_CHANGED, [this](const UI::EditorEvent& event) {
        if (std::holds_alternative<int>(event.data)) {
            handleToolChanged(std::get<int>(event.data));
        }
    });
    
    UI::g_eventDispatcher.subscribe(UI::EditorEventType::FILE_NEW, [this](const UI::EditorEvent& event) {
        handleFileAction(UI::EditorEventType::FILE_NEW);
    });
    
    UI::g_eventDispatcher.subscribe(UI::EditorEventType::FILE_SAVE, [this](const UI::EditorEvent& event) {
        handleFileAction(UI::EditorEventType::FILE_SAVE);
    });
    
    UI::g_eventDispatcher.subscribe(UI::EditorEventType::FILE_OPEN, [this](const UI::EditorEvent& event) {
        handleFileAction(UI::EditorEventType::FILE_OPEN);
    });
    
    UI::g_eventDispatcher.subscribe(UI::EditorEventType::OBJECT_DELETED, [this](const UI::EditorEvent& event) {
        if (std::holds_alternative<int>(event.data)) {
            int objectId = std::get<int>(event.data);
            if (objectId >= 0 && objectId < _objects3D.size()) {
                auto it = _objects3D.begin() + objectId;
                removeCube(it);
            }
        }
    });
    
    UI::g_eventDispatcher.subscribe(UI::EditorEventType::ASSET_SELECTED, [this](const UI::EditorEvent& event) {
        if (std::holds_alternative<int>(event.data)) {
            handleAssetSelected(std::get<int>(event.data));
        }
    });
    
    UI::g_eventDispatcher.subscribe(UI::EditorEventType::GRID_TOGGLED, [this](const UI::EditorEvent& event) {
        if (std::holds_alternative<bool>(event.data)) {
            _gridVisible = std::get<bool>(event.data);
        }
    });
}

void MapEditor::handleToolChanged(int toolIndex)
{
    _currentTool = toolIndex;
    std::cout << "Tool changed to: " << toolIndex << std::endl;
    
    // Adjust behavior based on tool
    switch (toolIndex) {
        case 0: // SELECT
            // Enable selection mode
            break;
        case 1: // HAND
            // Enable camera movement mode
            break;
        case 2: // PEN
            // Enable drawing mode (for 2D editor primarily)
            break;
        case 3: // ERASER
            // Enable eraser mode
            break;
        case 4: // CUBE
            // Enable cube placement mode
            break;
        case 5: // ZOOM
            // Enable zoom mode
            break;
    }
}

void MapEditor::handleFileAction(UI::EditorEventType actionType, const std::string& filepath)
{
    switch (actionType) {
        case UI::EditorEventType::FILE_NEW:
            // Clear current scene
            _objects3D.clear();
            _objects2D.clear();
            notifySceneChanged();
            std::cout << "New scene created" << std::endl;
            break;
        case UI::EditorEventType::FILE_SAVE:
            saveMapBinary("game_project/assets/maps/game_map.dat");
            std::cout << "Map saved" << std::endl;
            break;
        case UI::EditorEventType::FILE_OPEN:
            loadMapBinary("game_project/assets/maps/game_map.dat");
            notifySceneChanged();
            std::cout << "Map loaded" << std::endl;
            break;
        case UI::EditorEventType::FILE_EXPORT:
            gameCompilation("game_project");
            std::cout << "Game exported" << std::endl;
            break;
        default:
            break;
    }
}

void MapEditor::handleAssetSelected(int assetIndex)
{
    // Change current asset type based on selection
    std::cout << "Asset selected: " << assetIndex << std::endl;
    // This would change _currentCubeType or _currentSpireType based on the asset
}

int MapEditor::getObjectCount() const
{
    return _objects3D.size() + _objects2D.size();
}

int MapEditor::getSelectedObjectId() const
{
    return _selectedObjectId;
}

std::string MapEditor::getSelectedObjectName() const
{
    if (_selectedObjectId >= 0 && _selectedObjectId < _objects3D.size()) {
        return "Cube" + std::to_string(_selectedObjectId);
    }
    return "";
}

Vector3 MapEditor::getCameraPosition() const
{
    return _camera.getPosition().convert();
}

bool MapEditor::isGridVisible() const
{
    return _gridVisible;
}

// ISceneProvider interface implementation
std::vector<UI::SceneObjectInfo> MapEditor::getSceneObjects() const
{
    std::vector<UI::SceneObjectInfo> sceneObjects;
    
    // Add 3D objects (cubes)
    for (size_t i = 0; i < _objects3D.size(); ++i) {
        const BasicObject& obj = _objects3D[i];
        Vector3D pos = const_cast<BasicObject&>(obj).getBox().position;
        
        UI::SceneObjectInfo objInfo(
            static_cast<int>(i),
            "Cube_" + std::to_string(i),
            UI::SceneObjectType::CUBE_3D,
            pos.convert(),
            {0, 0, 0}, // rotation - could get from object if available
            {1, 1, 1}  // scale - could get from object if available
        );
        
        objInfo.isSelected = (static_cast<int>(i) == _selectedObjectId);
        sceneObjects.push_back(objInfo);
    }
    
    // Add 2D objects (sprites)
    for (size_t i = 0; i < _objects2D.size(); ++i) {
        const BasicObject& obj = _objects2D[i];
        Vector3D pos = const_cast<BasicObject&>(obj).getBox().position;
        
        UI::SceneObjectInfo objInfo(
            static_cast<int>(_objects3D.size() + i),
            "Sprite_" + std::to_string(i),
            UI::SceneObjectType::SPRITE_2D,
            pos.convert(),
            {0, 0, 0},
            {1, 1, 1}
        );
        
        objInfo.isSelected = (static_cast<int>(_objects3D.size() + i) == _selectedObjectId);
        sceneObjects.push_back(objInfo);
    }
    
    // Add camera as a scene object
    UI::SceneObjectInfo cameraInfo(
        static_cast<int>(_objects3D.size() + _objects2D.size()),
        "Main_Camera",
        UI::SceneObjectType::CAMERA,
        getCameraPosition(),
        {0, 0, 0},
        {1, 1, 1}
    );
    sceneObjects.push_back(cameraInfo);
    
    return sceneObjects;
}

UI::SceneObjectInfo MapEditor::getObjectInfo(int objectId) const
{
    // Check if it's a 3D object
    if (objectId >= 0 && objectId < static_cast<int>(_objects3D.size())) {
        const BasicObject& obj = _objects3D[objectId];
        Vector3D pos = const_cast<BasicObject&>(obj).getBox().position;
        
        return UI::SceneObjectInfo(
            objectId,
            "Cube_" + std::to_string(objectId),
            UI::SceneObjectType::CUBE_3D,
            pos.convert(),
            {0, 0, 0},
            {1, 1, 1}
        );
    }
    
    // Check if it's a 2D object
    int sprite2DIndex = objectId - static_cast<int>(_objects3D.size());
    if (sprite2DIndex >= 0 && sprite2DIndex < static_cast<int>(_objects2D.size())) {
        const BasicObject& obj = _objects2D[sprite2DIndex];
        Vector3D pos = const_cast<BasicObject&>(obj).getBox().position;
        
        return UI::SceneObjectInfo(
            objectId,
            "Sprite_" + std::to_string(sprite2DIndex),
            UI::SceneObjectType::SPRITE_2D,
            pos.convert(),
            {0, 0, 0},
            {1, 1, 1}
        );
    }
    
    // Check if it's the camera
    int cameraId = static_cast<int>(_objects3D.size() + _objects2D.size());
    if (objectId == cameraId) {
        return UI::SceneObjectInfo(
            objectId,
            "Main_Camera",
            UI::SceneObjectType::CAMERA,
            getCameraPosition(),
            {0, 0, 0},
            {1, 1, 1}
        );
    }
    
    // Return default if not found
    return UI::SceneObjectInfo();
}

bool MapEditor::selectObject(int objectId)
{
    // Check if it's a valid 3D object
    if (objectId >= 0 && objectId < static_cast<int>(_objects3D.size())) {
        _selectedObjectId = objectId;
        UI::Events::objectSelected(objectId);
        return true;
    }
    
    // Check if it's a valid 2D object
    int sprite2DIndex = objectId - static_cast<int>(_objects3D.size());
    if (sprite2DIndex >= 0 && sprite2DIndex < static_cast<int>(_objects2D.size())) {
        _selectedObjectId = objectId;
        UI::Events::objectSelected(objectId);
        return true;
    }
    
    // Check if it's the camera
    int cameraId = static_cast<int>(_objects3D.size() + _objects2D.size());
    if (objectId == cameraId) {
        _selectedObjectId = objectId;
        UI::Events::objectSelected(objectId);
        return true;
    }
    
    // Invalid object ID
    return false;
}

bool MapEditor::deleteObject(int objectId)
{
    // Check if it's a 3D object
    if (objectId >= 0 && objectId < static_cast<int>(_objects3D.size())) {
        auto it = _objects3D.begin() + objectId;
        removeCube(it);
        
        // Update selected object ID if needed
        if (_selectedObjectId == objectId) {
            _selectedObjectId = -1;
        } else if (_selectedObjectId > objectId) {
            _selectedObjectId--;
        }
        
        notifySceneChanged();
        return true;
    }
    
    // Check if it's a 2D object
    int sprite2DIndex = objectId - static_cast<int>(_objects3D.size());
    if (sprite2DIndex >= 0 && sprite2DIndex < static_cast<int>(_objects2D.size())) {
        auto it = _objects2D.begin() + sprite2DIndex;
        removePlayer(it);
        
        // Update selected object ID if needed
        if (_selectedObjectId == objectId) {
            _selectedObjectId = -1;
        }
        
        notifySceneChanged();
        return true;
    }
    
    // Cannot delete camera
    return false;
}

void MapEditor::updateSceneObjects()
{
    // This method can be called to force a scene update
    notifySceneChanged();
}

void MapEditor::notifySceneChanged()
{
    UI::Events::sceneUpdated();
}
