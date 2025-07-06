#include "iostream"
#include <filesystem>

#include "3DMapEditor.hpp"
#include "Input/InputTypes.hpp"

MapEditor::MapEditor() : _grid(), _cubeHeight(1.0f), _closestObject(std::nullopt), _closestSprite(std::nullopt),
                        _cursorPosition(0, 0), _alignedPosition(0, 0.5f, 0),
                        _placePlayer(false), _drawWireframe(false) 
{
}

MapEditor::~MapEditor()
{
}

void MapEditor::init(std::shared_ptr<Render::Window> window, std::shared_ptr<Render::Camera> camera)
{
    _window = window;
    _camera = camera;

    initGrid();

    setupEventHandlers();
}

void MapEditor::initGrid()
{
    _grid.init();
}

void MapEditor::update(input::IHandlerBase &inputHandler)
{
    if (!_camera) {
        std::cerr << "[ERROR] MapEditor::_camera is null in update()\n";
        return;
    }
    _cursorPosition = inputHandler.getCursorCoords();
    updateCursor();;

    if (inputHandler.isReleased(input::Generic::SELECT1)) {
        if (_currentTool == 4 && _alignedPosition != Vector3D(0, 0, 0) && _blocSelect) { // CUBE tool
            addCube(_alignedPosition);
            UI::Events::objectCreated(_alignedPosition.convert());
            notifySceneChanged();
        } else if (_currentTool == 4 && _alignedPosition != Vector3D(0, 0, 0) && !_blocSelect) {
            addPlayer(_alignedPosition);
            UI::Events::objectCreated(_alignedPosition.convert());
            notifySceneChanged();
        }
    }
    if (inputHandler.isReleased(input::Generic::SELECT2)) {
        if (!_objects3D.empty() && _closestObject.has_value()) {
            removeCube(_closestObject.value());
            notifySceneChanged();
        } else if (!_objects2D.empty() &&  _closestSprite.has_value()) {
            removePlayer(_closestSprite.value());
            notifySceneChanged();
        }
    }
    if (inputHandler.isReleased(input::Generic::LEFT)) {
        _camera->rotateClock();
        UI::Events::cameraMove(_camera->getPosition().convert());
        std::cout << "Rotate Camera" << std::endl;
    }
    if (inputHandler.isReleased(input::Generic::RIGHT)) {
        _camera->rotateCounterclock();

        std::cout << "Other Rotate Camera" << std::endl;
    }
    if (inputHandler.isPressed(input::Generic::DOWN)) {
        saveMap("game_project/assets/maps/game_map.dat");
        std::cout << "Save map" << std::endl;
    }
    if (inputHandler.isPressed(input::Generic::UP)) {
        loadMap("game_project/assets/maps/game_map.dat");
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

void MapEditor::draw2DElements(Rectangle mainViewArea, std::shared_ptr<Render::Camera> camera)
{
    for (auto i = _objects2D.begin(); i != _objects2D.end(); i++) {
        i->get()->draw(mainViewArea, camera);
    }
    Vector2 aligned = { _alignedPosition.convert().x, _alignedPosition.convert().y };
}

void MapEditor::draw3DElements()
{
    _grid.draw();
    for (auto i = _objects3D.begin(); i != _objects3D.end(); i++) {
        i->get()->draw();
    }

    if (_currentTool == 4) {
        Vector3 size = (Vector3){ _cubeHeight, _cubeHeight, _cubeHeight };
        Color color = (Color){ 255, 255, 0, 128 }; // Yellow with 50% opacity
        DrawCubeV(_alignedPosition.convert(), size, color); // Draw preview cube
    }
}

void MapEditor::draw(Rectangle mainViewArea, std::shared_ptr<Render::Camera> camera)
{
    // Draw 3D elements
    BeginScissorMode(mainViewArea.x, mainViewArea.y, mainViewArea.width, mainViewArea.height);
    _camera->start3D();
    draw3DElements();
    _camera->end3D();
    EndScissorMode();
    // Draw 2D elements
    draw2DElements(mainViewArea, camera);
}

void MapEditor::changeCubeType(Asset3D newAsset)
{
    _currentCubeType = newAsset;
}

void MapEditor::changeTextureType(Asset2D newAsset)
{
    _currentTextureType = newAsset;
}

void MapEditor::changeSpriteType(Asset2D newAsset)
{
    _currentSpriteType = newAsset;
}

void MapEditor::addCube(Vector3D position)
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

    std::shared_ptr<MapElement> newCube = std::make_shared<MapElement>(_currentCubeType, position, Vector3D(_cubeHeight, _cubeHeight, _cubeHeight));
    std::cout << "ADD NEW CUBE POS: " << position.x << " " << position.y << " " << position.z << std::endl;
    _objects3D.push_back(newCube);
    updateCursor();
}

void MapEditor::addPlayer(Vector3D position)
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

    std::shared_ptr<Character> newCharacter = std::make_shared<Character>(_currentSpriteType);
    newCharacter->setBox3DPosition(position);
    std::cout << "ADD NEW PLAYER POS: " << position << std::endl;
    std::cout << "[SCALE NEW PLAYER]: " << _currentSpriteType.getScale() << std::endl;
    newCharacter->setBox2DSize({_currentSpriteType.getWidth(), _currentSpriteType.getHeight()});
    newCharacter->setBox2DScale(_currentSpriteType.getScale());
    _spriteSize = {_currentSpriteType.getWidth(), _currentSpriteType.getHeight()};
    _objects2D.push_back(newCharacter);
}

void MapEditor::removeCube(std::vector<std::shared_ptr<MapElement>>::iterator toRemove)
{
    _objects3D.erase(toRemove);
}

void MapEditor::removePlayer(std::vector<std::shared_ptr<Character>>::iterator toRemove)
{
    _objects2D.erase(toRemove);
}

void MapEditor::findPositionFromHit(RayCollision &hit)
{
    Vector3D collisionPoint = hit.point;
    Vector3D modelPos = _closestObject.value()->get()->getBox3D().getPosition();
    Vector3D diff = collisionPoint - modelPos;
    Vector3D alignedPos = modelPos;

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

void MapEditor::updateCursor()
{
    Ray ray = GetMouseRay(_cursorPosition.convert(), _camera->getRaylibCam());
    RayCollision closestHit = { false, std::numeric_limits<float>::max(), { 0, 0, 0 }, { 0, 0, 0 } };

    _closestObject = std::nullopt;
    _closestSprite = std::nullopt;

    // check if we are over an object
    for (auto i = _objects3D.begin(); i != _objects3D.end(); i++) {
        RayCollision collision = GetRayCollisionBox(ray, i->get()->getBox3D().convert());
        if (collision.hit && collision.distance < closestHit.distance) {
            closestHit = collision;
            _closestObject = i;
        }
    }

    for (auto i = _objects2D.begin(); i != _objects2D.end(); i++) {
        RayCollision collision = GetRayCollisionBox(ray, i->get()->getBox3D().convert());
        if (collision.hit && collision.distance < closestHit.distance) {
            closestHit = collision;
            _closestSprite = i;
        }
    }

    // if there was a hit, we adapt the position of the cube to be placed
    if (_closestObject.has_value()) {
        findPositionFromHit(closestHit);
    } else { // otherwise, we check if we are over the grid
        findPositionFromGrid(ray);
    }
}

void MapEditor::saveMap(const std::string& filename)
{
    std::filesystem::path filepath(filename);
    std::filesystem::create_directories(filepath.parent_path());

    std::ofstream file(filename, std::ios::out);
    if (!file.is_open()) {
        std::cerr << "Failed to open or create map file for saving: " << filename << "\n";
        return;
    }

    Vector3D pos3D = Vector3D();
    Vector3D pos2D = Vector3D();

    file << "MAP\n";
    file << _objects3D.size() << "\n";
    for (auto& obj : _objects3D) {
        pos3D = obj->getBox3D().getPosition();
        file << pos3D.x << " " << pos3D.y << " " << pos3D.z << " " << obj->getAsset3D().getFileName() << " " << obj->getAsset3D().getScale() << "\n";
    }

    file << "PLAYER\n";
    file << _objects2D.size() << "\n";
    for (auto& obj : _objects2D) {
        pos2D = obj->getBox3D().getPosition();
        file << pos2D.x << " " << pos2D.y << " " << pos2D.z << " " << obj->getAsset2D().getFileName() << " " << obj->getBox2D().getSize().x << " " << obj->getBox2D().getSize().y << " " << obj->getAsset2D().getScale() << "\n";
    }

    file.close();
    std::cout << "Map saved to: " << filename << "\n";
}

void MapEditor::loadMap(const std::string& filename)
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
        for (int i = 0; i < count2; ++i) {
            file >> position.x >> position.y >> position.z >> filePath >> size.x >> size.y >> scale;
            std::cout << "FILENAME " << filePath << "\n";
            std::cout << "POSITION: " << position << "\n";
            std::cout << "SIZE: " << size << "\n";
            std::cout << "SCALE: " << scale << "\n";
            Asset2D tmpAsset(filePath);
            tmpAsset.loadFile();
            tmpAsset.setScale(scale);
            tmpAsset.setWidth(size.x);
            tmpAsset.setHeight(size.y);
            changeSpriteType(tmpAsset);
            addPlayer(position);
            std::cout << "ADD NEW PLAYER : " << i << "\n";
        }
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
            if (objectId >= 0 && objectId < _objects3D.size() && _blocSelect) {
                auto it = _objects3D.begin() + objectId;
                removeCube(it);
            } else if (objectId >= 0 && objectId < _objects2D.size() && !_blocSelect) {
                auto it = _objects2D.begin() + objectId;
                removePlayer(it);
            }
        }
    });

    UI::g_eventDispatcher.subscribe(UI::EditorEventType::ASSET_SELECTED,
        [this](const UI::EditorEvent& event) {
            if (std::holds_alternative<std::shared_ptr<AAsset>>(event.data)) {
                std::shared_ptr<AAsset> asset = std::get<std::shared_ptr<AAsset>>(event.data);
                handleAssetSelected(asset);
            }
            
        });

    UI::g_eventDispatcher.subscribe(UI::EditorEventType::ASSET_LOADED, [this](const UI::EditorEvent& event) {
        if (std::holds_alternative<int>(event.data)) {
            handleAssetLoaded();
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
            saveMap("game_project/assets/maps/game_map.dat");
            std::cout << "Map saved" << std::endl;
            break;
        case UI::EditorEventType::FILE_OPEN:
            loadMap("game_project/assets/maps/game_map.dat");
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

void MapEditor::handleAssetSelected(std::shared_ptr<AAsset> asset)
{
    Asset3D *asset3D = dynamic_cast<Asset3D*>(asset.get());
    Asset2D *asset2D = dynamic_cast<Asset2D*>(asset.get());

    if (asset3D != nullptr) {
        _blocSelect = true;
        changeCubeType(*asset3D);
        std::cout << "Asset selected 3D" << std::endl;
    } else if (asset2D != nullptr) {
        _blocSelect = false;
        changeSpriteType(*asset2D);
        std::cout << "Asset selected 2D" << std::endl;
    } else {
        std::cerr << "Unknown asset type selected!" << std::endl;
    }
}

void MapEditor::handleAssetLoaded()
{
    std::cout << "LOAD ASSET ATTENTION" << std::endl;
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
    return _camera->getPosition().convert();
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
        const MapElement *obj = _objects3D[i].get();
        Vector3D pos = const_cast<MapElement*>(obj)->getBoxPosition();
        
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
        const Character *obj = _objects2D[i].get();
        Vector3D pos = const_cast<Character*>(obj)->getBoxPosition();
        
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
        const MapElement *obj = _objects3D[objectId].get();
        Vector3D pos = const_cast<MapElement*>(obj)->getBoxPosition();
        
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
        const Character *obj = _objects2D[objectId].get();
        Vector3D pos = const_cast<Character*>(obj)->getBoxPosition();
        
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
    if (objectId >= 0 && objectId < static_cast<int>(_objects3D.size()) && _blocSelect) {
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
    if (sprite2DIndex >= 0 && sprite2DIndex < static_cast<int>(_objects2D.size())  && !_blocSelect) {
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

void MapEditor::setLoader(std::shared_ptr<AssetLoader> loader)
{
    _loader = loader;
    _objects3DLoaded = _loader->getLoaded3DAssets();
    _objects2DLoaded = _loader->getLoaded2DAssets();
}
