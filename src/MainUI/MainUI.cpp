#include "MainUI.hpp"
#include <thread>
#include <chrono>

MainUI::MainUI(std::shared_ptr<Render::Camera> camera, std::shared_ptr<Render::Window> window) : _uiManager(SCREENWIDTH, SCREENHEIGHT) 
{
    _camera = camera;
    _window = window;
    _window.get()->startWindow(Vector2D(SCREENWIDTH, SCREENHEIGHT));
    
    // Initialize editors
    _3DMapEditor.init(window, camera);
    _scriptingEditor.init(window, camera);
    _uiManager.initialize();

    _gameProjectName = "game_project";
    _currentEditor = SCRIPTING; // Default to Map Editor
    
    _loader = std::make_shared<AssetLoader>();
    _3DMapEditor.setLoader(_loader);
    _uiManager.setLoader(_loader);
    //temporary cube asset loading for the 3D map, to change after libraries are implemented
    initMapEditorAssets();
    
    // Setup event handlers for UI-MainUI communication
    setupEventHandlers();
}

MainUI::~MainUI()
{
}

void MainUI::initMapEditorAssets()
{
    Color myColor = LIGHTGRAY;
    Image colorImage = GenImageColor(1, 1, myColor);
    Texture2D colorTexture = LoadTextureFromImage(colorImage);
    UnloadImage(colorImage);
    Asset2D textureAsset(colorTexture);
    // Asset2D textureAsset("ressources/textures/cube_texture.png")
    _3DMapEditor.changeTextureType(textureAsset);

    Asset3D cubeAsset("ressources/elements/models/cube.obj");
    if (textureAsset.isLoaded())
        cubeAsset.setModelTexture(textureAsset.getTexture());
    _3DMapEditor.changeCubeType(cubeAsset);

    Asset2D playerAsset;
    playerAsset.setFileName("ressources/shy_guy_red.png");
    playerAsset.loadFile();
    _3DMapEditor.changeSpriteType(playerAsset);
}

void MainUI::update(input::IHandlerBase &inputHandler) {
    Vector2D cursorPos = inputHandler.getCursorCoords();

    _loader->updateAssets("ressources/loadedAssets");
    _3DMapEditor.setLoader(_loader);
    _uiManager.setLoader(_loader);
    
    // Update current editor
    switch (_currentEditor) {
        case MAP:
            _3DMapEditor.update(inputHandler);
            break;
        case SCRIPTING:
            _scriptingEditor.update(inputHandler);
            break;
        case PAINT:
            // 2D Editor update would go here
            break;
    }
    
    _uiManager.update(inputHandler);
}

void MainUI::draw() {
    _window.get()->startRender();

    // Get the main view area from the UI manager
    Rectangle mainViewArea = _uiManager.getMainViewArea();

    // Draw content in the main view area
    _window.get()->clearBackground(UI::BACKGROUND);

    // Draw current editor content
    switch (_currentEditor) {
        case MAP:
            _3DMapEditor.draw(mainViewArea, _camera);
            _uiManager.draw(_3DMapEditor);
            break;
        case SCRIPTING:
            _scriptingEditor.setSceneProvider(&_3DMapEditor); // Use map editor as scene provider
            _scriptingEditor.draw(mainViewArea);
            _uiManager.draw(_scriptingEditor);
            break;
        case PAINT:
            // 2D Editor draw would go here
            // For now, just draw UI with map editor as fallback
            _uiManager.draw(_3DMapEditor); // Fallback
            break;
    }
    
    _window.get()->endRender();
}

void MainUI::loop(input::IHandlerBase &inputHandler) {
    while (!_window.get()->isWindowClosing()) {
        update(inputHandler);
        draw();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    _window.get()->closeWindow();
}

void MainUI::setCurrentEditor(EditorType editorType) {
    _currentEditor = editorType;
    std::cout << "[MainUI] Switched to editor type: " << editorType << std::endl;
}

EditorType MainUI::getCurrentEditor() const {
    return _currentEditor;
}

void MainUI::setupEventHandlers() {
    // Subscribe to editor mode change events from the UI
    UI::g_eventDispatcher.subscribe(UI::EditorEventType::EDITOR_MODE_CHANGED, 
        [this](const UI::EditorEvent& event) {
            if (std::holds_alternative<int>(event.data)) {
                int editorIndex = std::get<int>(event.data);
                EditorType newEditor;
                
                // Map UI editor indices to EditorType enum
                switch (editorIndex) {
                    case 0: newEditor = PAINT; break;
                    case 1: newEditor = MAP; break;
                    case 2: newEditor = SCRIPTING; break;
                    default: 
                        std::cout << "[MainUI] Unknown editor index: " << editorIndex << std::endl;
                        return;
                }
                
                setCurrentEditor(newEditor);
            }
        });
    
    std::cout << "[MainUI] Event handlers set up" << std::endl;
}
