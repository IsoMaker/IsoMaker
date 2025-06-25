#include "UIManager.hpp"
#include <iostream>

namespace UI {

UIManager::UIManager(int screenWidth, int screenHeight)
    : _screenWidth(screenWidth),
      _screenHeight(screenHeight),
      _currentTool(ToolType::SELECT),
      _show3DAssets(true),
      _selectedAssetIndex2D(-1),
      _selectedAssetIndex3D(-1),
      _selectedObjectIndex(-1),
      _transformSectionOpen(true),
      _lightingSectionOpen(false),
      _physicsSectionOpen(false),
      _position({0, 0, 0}),
      _rotation({0, 0, 0}),
      _scale({1, 1, 1}),
      _fileMenuOpen(false),
      _editMenuOpen(false),
      _renderMenuOpen(false),
      _helpMenuOpen(false),
      _searchActive(false),
      _currentEditorType(0),
      _currentSceneProvider(nullptr)
{
    // Initialize search text
    _searchText[0] = '\0';
    
    // Initialize panel visibility
    _panelVisibility[PanelType::SCENE] = true;
    _panelVisibility[PanelType::PROPERTIES] = true;
}

UIManager::~UIManager()
{
    // Clean up event subscriptions for this UIManager instance
    // Note: We don't clear the entire dispatcher as other components might still be using it
    
    // Unload icons and textures
    unloadIcons();
}

void UIManager::initialize()
{
    // Set custom gui style
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(UI_TEXT_PRIMARY));
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, ColorToInt(UI_TEXT_PRIMARY));
    GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, ColorToInt(UI_TEXT_PRIMARY));
    GuiSetStyle(DEFAULT, TEXT_COLOR_DISABLED, ColorToInt(UI_TEXT_SECONDARY));
    
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, ColorToInt(UI_SECONDARY));
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, ColorToInt(UI_PRIMARY));
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, ColorToInt(ACCENT_PRIMARY));
    
    GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, ColorToInt(UI_SECONDARY));
    GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED, ColorToInt(ACCENT_TERTIARY));
    GuiSetStyle(DEFAULT, BORDER_COLOR_PRESSED, ColorToInt(ACCENT_PRIMARY));
    
    // Load icons
    loadIcons();
    loadDefaultAsset("ressources/");
    
    // Setup event handlers
    setupEventHandlers();
}

void UIManager::update(input::IHandlerBase &inputHandler)
{
    Vector2 mousePos = { static_cast<float>(inputHandler.getCursorCoords().x), 
                         static_cast<float>(inputHandler.getCursorCoords().y) };
    
    // Check for clicks outside menus to close them
    if (inputHandler.isPressed(input::Generic::SELECT1)) {
        // Close menus if clicked outside
        if (_fileMenuOpen && mousePos.y > 30) _fileMenuOpen = false;
        if (_editMenuOpen && mousePos.y > 30) _editMenuOpen = false;
        if (_renderMenuOpen && mousePos.y > 30) _renderMenuOpen = false;
        if (_helpMenuOpen && mousePos.y > 30) _helpMenuOpen = false;
    }
    
    // Check tool selection in left toolbar
    if (inputHandler.isPressed(input::Generic::SELECT1)) {
        if (CheckCollisionPointRec(mousePos, {0.0f, static_cast<float>(_topBarHeight), static_cast<float>(_leftToolbarWidth), 
                                             static_cast<float>(_screenHeight - _topBarHeight - _bottomAssetsBarHeight)})) {
            int toolIndex = (mousePos.y - _topBarHeight) / 60; // Each tool is 60px tall
            if (toolIndex >= 0 && toolIndex < 6) { // 6 tools
                _currentTool = static_cast<ToolType>(toolIndex);
            }
        }
    }
    
    // Handle keyboard shortcuts for tools
    if (IsKeyPressed(KEY_S)) { _currentTool = ToolType::SELECT; Events::toolChanged(0); }
    if (IsKeyPressed(KEY_H)) { _currentTool = ToolType::HAND; Events::toolChanged(1); }
    if (IsKeyPressed(KEY_P)) { _currentTool = ToolType::PEN; Events::toolChanged(2); }
    if (IsKeyPressed(KEY_E)) { _currentTool = ToolType::ERASER; Events::toolChanged(3); }
    if (IsKeyPressed(KEY_C)) { _currentTool = ToolType::CUBE; Events::toolChanged(4); }
    if (IsKeyPressed(KEY_Z)) { _currentTool = ToolType::ZOOM; Events::toolChanged(5); }
}

void UIManager::draw(MapEditor &mapEditor)
{
    // Update scene provider reference
    if (_currentSceneProvider != &mapEditor) {
        _currentSceneProvider = &mapEditor;
        refreshSceneObjects();
    }
    
    // Draw UI components
    drawLeftToolbar();
    drawTopMenuBar();
    drawRightPanels();
    drawBottomAssetsBar();
    
    // Draw main view area border
    Rectangle mainViewArea = getMainViewArea();
    DrawRectangleLinesEx(mainViewArea, 1, UI_SECONDARY);
}

void UIManager::drawTopMenuBar()
{
    // Draw top bar background
    DrawRectangle(0, 0, static_cast<int>(_screenWidth), static_cast<int>(_topBarHeight), UI_PRIMARY);
    
    // Menu items
    int menuItemWidth = 80;
    
    // File menu
    Rectangle fileRect = {10.0f, 0.0f, static_cast<float>(menuItemWidth), static_cast<float>(_topBarHeight)};
    if (MenuItem(fileRect, "File", &_fileMenuOpen)) {
        _editMenuOpen = false;
        _renderMenuOpen = false;
        _helpMenuOpen = false;
    }
    
    // File submenu
    if (_fileMenuOpen) {
        const char* fileItems[5] = {"New", "Open", "Save", "Export", "Exit"};
        int fileItemSelected = Submenu({fileRect.x, fileRect.y + fileRect.height, 120.0f, 125.0f}, fileItems, 5);
        
        // Handle file menu selection
        if (fileItemSelected >= 0) {
            _fileMenuOpen = false;
            handleFileMenuAction(fileItemSelected);
        }
    }
    
    // Edit menu
    Rectangle editRect = {static_cast<float>(10 + menuItemWidth), 0.0f, static_cast<float>(menuItemWidth), static_cast<float>(_topBarHeight)};
    if (MenuItem(editRect, "Edit", &_editMenuOpen)) {
        _fileMenuOpen = false;
        _renderMenuOpen = false;
        _helpMenuOpen = false;
    }
    
    // Edit submenu
    if (_editMenuOpen) {
        const char* editItems[6] = {"Undo", "Redo", "Cut", "Copy", "Paste", "Delete"};
        int editItemSelected = Submenu({editRect.x, editRect.y + editRect.height, 120.0f, 150.0f}, editItems, 6);
        
        // Handle edit menu selection
        if (editItemSelected >= 0) {
            _editMenuOpen = false;
            handleEditMenuAction(editItemSelected);
        }
    }
    
    // Render menu
    Rectangle renderRect = {static_cast<float>(10 + menuItemWidth * 2), 0.0f, static_cast<float>(menuItemWidth), static_cast<float>(_topBarHeight)};
    if (MenuItem(renderRect, "Render", &_renderMenuOpen)) {
        _fileMenuOpen = false;
        _editMenuOpen = false;
        _helpMenuOpen = false;
    }
    
    // Render submenu
    if (_renderMenuOpen) {
        const char* renderItems[4] = {"Preview", "Grid On/Off", "Shadows", "Quality"};
        int renderItemSelected = Submenu({renderRect.x, renderRect.y + renderRect.height, 120.0f, 100.0f}, renderItems, 4);
        
        // Handle render menu selection
        if (renderItemSelected >= 0) {
            _renderMenuOpen = false;
            handleRenderMenuAction(renderItemSelected);
        }
    }
    
    // Help menu
    Rectangle helpRect = {static_cast<float>(10 + menuItemWidth * 3), 0.0f, static_cast<float>(menuItemWidth), static_cast<float>(_topBarHeight)};
    if (MenuItem(helpRect, "Help", &_helpMenuOpen)) {
        _fileMenuOpen = false;
        _editMenuOpen = false;
        _renderMenuOpen = false;
    }
    
    // Help submenu
    if (_helpMenuOpen) {
        const char* helpItems[3] = {"Documentation", "Shortcuts", "About"};
        int helpItemSelected = Submenu({helpRect.x, helpRect.y + helpRect.height, 120.0f, 75.0f}, helpItems, 3);
        
        // Handle help menu selection
        if (helpItemSelected >= 0) {
            _helpMenuOpen = false;
            handleHelpMenuAction(helpItemSelected);
        }
    }
    
    // Editor dropdown (right aligned)
    int dropdownWidth = 150;
    int dropdownPos = _screenWidth - dropdownWidth - 10;
    
    // Dropdown to select editor type
    const char *editorTypes[2] = { "Map Editor", "Scripting" };
    
    // Use the custom dropdown
    Rectangle dropdownBounds = {static_cast<float>(dropdownPos), 2.0f, static_cast<float>(dropdownWidth), static_cast<float>(_topBarHeight - 4)};
    bool editMode = false;
    int newEditorType = CustomDropdown(dropdownBounds, editorTypes[_currentEditorType], _currentEditorType, editorTypes, 2, &editMode);
    
    // Check if editor type changed
    if (newEditorType != _currentEditorType) {
        _currentEditorType = newEditorType;
        Events::editorModeChanged(_currentEditorType);
    }
    
    // Search bar (middle)
    Rectangle searchBounds = {
        static_cast<float>(_screenWidth / 2 - 100),
        5,
        200,
        20
    };
    SearchBar(searchBounds, _searchText, sizeof(_searchText), &_searchActive);
}

void UIManager::drawLeftToolbar()
{
    // Draw toolbar background
    DrawRectangle(0, static_cast<int>(_topBarHeight), static_cast<int>(_leftToolbarWidth), 
                 static_cast<int>(_screenHeight - _topBarHeight - _bottomAssetsBarHeight), UI_PRIMARY);
    
    // Tool definitions
    const char* toolTips[6] = {
        "Select Tool (S)",
        "Hand Tool (H)",
        "Pen Tool (P)",
        "Eraser Tool (E)",
        "Cube Tool (C)",
        "Zoom Tool (Z)"
    };
    
    // Use ToolButton component for each tool
    for (int i = 0; i < 6; i++) {
        Rectangle toolBounds = {
            0,
            static_cast<float>(_topBarHeight + i * 60),
            static_cast<float>(_leftToolbarWidth),
            60
        };
        
        // Use actual tool icon if available
        Texture2D toolIcon = { 0 };
        if (i < _toolIcons.size() && _toolIcons[i].id > 0) {
            toolIcon = _toolIcons[i];
        }
        
        if (ToolButton(toolBounds, toolIcon, toolTips[i], static_cast<int>(_currentTool) == i)) {
            _currentTool = static_cast<ToolType>(i);
            Events::toolChanged(i);
        }
    }
}

void UIManager::drawRightPanels()
{
    int panelHeight = (_screenHeight - _topBarHeight - _bottomAssetsBarHeight) / 2;
    int panelY = _topBarHeight;
    
    // Scene panel
    if (_panelVisibility[PanelType::SCENE]) {
        // Panel bounds
        Rectangle panelBounds = {
            static_cast<float>(_screenWidth - _rightPanelsWidth),
            static_cast<float>(panelY),
            static_cast<float>(_rightPanelsWidth),
            static_cast<float>(panelHeight)
        };
        
        // Use the CollapsiblePanel component - though always open for scene
        bool alwaysOpen = true;
        CollapsiblePanel(panelBounds, "Scene", &alwaysOpen, UI_PRIMARY, UI_SECONDARY);
        
        // Scene objects list
        for (int i = 0; i < _sceneObjects.size(); i++) {
            Rectangle itemBounds = {
                static_cast<float>(_screenWidth - _rightPanelsWidth + 5),
                static_cast<float>(panelY + 35 + i * 25),
                static_cast<float>(_rightPanelsWidth - 10),
                25
            };
            
            const SceneObjectInfo& obj = _sceneObjects[i];
            
            // Background for selected item
            if (obj.isSelected || i == _selectedObjectIndex) {
                DrawRectangleRec(itemBounds, ACCENT_TERTIARY);
            }
            
            // Add type indicator color
            Color typeColor = UI_TEXT_PRIMARY;
            switch (obj.type) {
                case SceneObjectType::CUBE_3D: typeColor = {100, 150, 255, 255}; break;
                case SceneObjectType::SPRITE_2D: typeColor = {255, 150, 100, 255}; break;
                case SceneObjectType::CAMERA: typeColor = {150, 255, 150, 255}; break;
                case SceneObjectType::LIGHT: typeColor = {255, 255, 100, 255}; break;
                default: typeColor = UI_TEXT_PRIMARY; break;
            }
            
            // Draw visibility indicator
            const char* visibilityIcon = obj.isVisible ? "👁" : "🚫";
            DrawText(visibilityIcon, itemBounds.x + 5, itemBounds.y + 5, 10, typeColor);
            
            // Draw object name
            DrawText(obj.getDisplayName().c_str(), itemBounds.x + 25, itemBounds.y + 5, 10, typeColor);
            
            // Check for selection
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), itemBounds)) {
                _selectedObjectIndex = i;
                if (_currentSceneProvider) {
                    _currentSceneProvider->selectObject(obj.id);
                }
                Events::objectSelected(obj.id);
            }
        }
    }
    
    // Properties panel
    if (_panelVisibility[PanelType::PROPERTIES]) {
        panelY += panelHeight;
        
        // Panel background
        DrawRectangle(static_cast<int>(_screenWidth - _rightPanelsWidth), static_cast<int>(panelY), static_cast<int>(_rightPanelsWidth), static_cast<int>(panelHeight), UI_SECONDARY);
        
        // Panel header
        DrawRectangle(static_cast<int>(_screenWidth - _rightPanelsWidth), static_cast<int>(panelY), static_cast<int>(_rightPanelsWidth), 30, UI_PRIMARY);
        DrawText("Properties", static_cast<int>(_screenWidth - _rightPanelsWidth + 10), static_cast<int>(panelY + 10), 10, UI_TEXT_PRIMARY);
        
        int sectionY = panelY + 35;
        int sectionHeight = 30;
        
        // Transform section
        Rectangle transformBounds = {
            static_cast<float>(_screenWidth - _rightPanelsWidth + 5),
            static_cast<float>(sectionY),
            static_cast<float>(_rightPanelsWidth - 10),
            static_cast<float>(sectionHeight)
        };
        
        // Use CollapsiblePanel component
        CollapsiblePanel(transformBounds, "Transform", &_transformSectionOpen, UI_PRIMARY, UI_SECONDARY);
        
        // Transform properties if section is open
        if (_transformSectionOpen) {
            int propertyY = sectionY + sectionHeight + 5;
            
            // Use PropertyEditor for position
            Rectangle positionBounds = {
                transformBounds.x + 5,
                static_cast<float>(propertyY),
                transformBounds.width - 10,
                60
            };
            _position = PropertyEditor(positionBounds, "Position", _position);
            
            // Use PropertyEditor for rotation
            Rectangle rotationBounds = {
                transformBounds.x + 5,
                static_cast<float>(propertyY + 50),
                transformBounds.width - 10,
                60
            };
            _rotation = PropertyEditor(rotationBounds, "Rotation", _rotation);
            
            // Use PropertyEditor for scale
            Rectangle scaleBounds = {
                transformBounds.x + 5,
                static_cast<float>(propertyY + 100),
                transformBounds.width - 10,
                60
            };
            _scale = PropertyEditor(scaleBounds, "Scale", _scale);
            
            // Update section Y for next section
            sectionY += 170;
        } else {
            sectionY += sectionHeight + 5;
        }
        
        // Lighting section
        Rectangle lightingBounds = {
            static_cast<float>(_screenWidth - _rightPanelsWidth + 5),
            static_cast<float>(sectionY),
            static_cast<float>(_rightPanelsWidth - 10),
            static_cast<float>(sectionHeight)
        };
    }
}

void UIManager::drawBottomAssetsBar()
{
    int barY = _screenHeight - _bottomAssetsBarHeight;
    
    // Draw assets bar background
    DrawRectangle(0, static_cast<int>(barY), static_cast<int>(_screenWidth), static_cast<int>(_bottomAssetsBarHeight), UI_SECONDARY);
    
    // Draw header
    Rectangle buttonRect3D = { 0, static_cast<float>(barY), 65, 30 };
    Color buttonColor3D = CheckCollisionPointRec(GetMousePosition(), buttonRect3D) ? UI_TEXT_SECONDARY : UI_TEXT_PRIMARY;

    DrawRectangleRec(buttonRect3D, UI_SECONDARY);
    DrawText("Assets 3D", 10, static_cast<int>(barY + 10), 10, buttonColor3D);

    if (CheckCollisionPointRec(GetMousePosition(), buttonRect3D) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        _show3DAssets = true;
        std::cout << "Assets 3D button clicked!" << std::endl;
        // UI::Events::assetSelected(0);
    }

    Rectangle buttonRect2D = { 70, static_cast<float>(barY), 65, 30 };
    Color buttonColor2D = CheckCollisionPointRec(GetMousePosition(), buttonRect2D) ? UI_TEXT_SECONDARY : UI_TEXT_PRIMARY;

    DrawRectangleRec(buttonRect2D, UI_SECONDARY);
    DrawText("Assets 2D", 80, static_cast<int>(barY + 10), 10, buttonColor2D);

    if (CheckCollisionPointRec(GetMousePosition(), buttonRect2D) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        _show3DAssets = false;
        std::cout << "Assets 2D button clicked!" << std::endl;
        // UI::Events::assetSelected(0);
    }

    Rectangle buttonRectPlus = {static_cast<float>(140), static_cast<float>(barY), static_cast<float>(30), static_cast<float>(30)};
    Color buttonColorPlus = CheckCollisionPointRec(GetMousePosition(), buttonRectPlus) ? UI_TEXT_SECONDARY : UI_TEXT_PRIMARY;
    
    DrawRectangleRec(buttonRectPlus, UI_SECONDARY);
    DrawText("+", 150, static_cast<int>(barY + 10), 10, buttonColorPlus); // Centered text
    
    if (CheckCollisionPointRec(GetMousePosition(), buttonRectPlus) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        std::cout << "+ button clicked! Toggling asset preview window." << std::endl;
    }

    if (_show3DAssets)
        drawBottomAssets3D(barY);
    else
        drawBottomAssets2D(barY);

}

void UIManager::drawBottomAssets2D(int barY)
{
    int assetSize = 80;
    int padding = 10;
    int rowCapacity = (_screenWidth - padding) / (assetSize + padding);
    
    for (int i = 0; i < 10; i++) { // Displaying 10 placeholder assets
        int row = i / rowCapacity;
        int col = i % rowCapacity;
        
        int x = padding + col * (assetSize + padding);
        int y = barY + 40 + row * (assetSize + padding);
        
        Rectangle assetBounds = {static_cast<float>(x), static_cast<float>(y), static_cast<float>(assetSize), static_cast<float>(assetSize)};
        
        // We would have real asset textures here
        Texture2D emptyTexture = { 0 };
        
        // Use AssetTile component
        char assetName[20];
        sprintf(assetName, "Asset %d", i + 1);
        
        if (AssetTile(assetBounds, emptyTexture, assetName, i == _selectedAssetIndex2D)) {
            _selectedAssetIndex2D = i;
            Events::assetSelected(i);
        }
    }
}

void UIManager::drawBottomAssets3D(int barY)
{
    int assetSize = 80;
    int padding = 10;
    int rowCapacity = (_screenWidth - padding) / (assetSize + padding);

    BeginScissorMode(0, barY, _screenWidth, _screenHeight - barY);

    for (int i = 0; i < _assetTiles3D.size(); i++) {
        int row = i / rowCapacity;
        int col = i % rowCapacity;

        int x = padding + col * (assetSize + padding);
        int y = barY + 40 + row * (assetSize + padding);

        Rectangle assetBounds = {static_cast<float>(x), static_cast<float>(y), static_cast<float>(assetSize), static_cast<float>(assetSize)};
        Rectangle tileBounds = { (float)x, (float)y, (float)assetSize, (float)assetSize };

        DrawRectangleRec(tileBounds, i == _selectedAssetIndex3D ? UI_PRIMARY : UI_SECONDARY);

        // Begin drawing to a tiny viewport
        rlViewport(x, _screenHeight - (y + assetSize), assetSize, assetSize);

        Camera cam = { 0 };
        cam.position = { 2.0f, 2.0f, 2.0f };
        cam.target = { 0.0f, 0.5f, 0.0f };
        cam.up = { 0.0f, 1.0f, 0.0f };
        cam.fovy = 45.0f;
        cam.projection = CAMERA_PERSPECTIVE;

        BeginMode3D(cam);

        if (_assetTiles3D[i].isLoaded()) {
            Vector3 pos = { 0.0f, 0.0f, 0.0f };
            Vector3 rotAxis = { 0.0f, 1.0f, 0.0f };
            float angle = GetTime() * 45.0f;
            Vector3 scale = { 0.5f, 0.5f, 0.5f };

            DrawModelEx(_assetTiles3D[i].getModel(), pos, rotAxis, angle, scale, WHITE);
        }

        EndMode3D();

        // Reset OpenGL state
        rlViewport(0, 0, _screenWidth, _screenHeight);
        rlLoadIdentity();  // Reset transformations

        // Selection
        if (CheckCollisionPointRec(GetMousePosition(), tileBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            _selectedAssetIndex3D = i;
            Events::assetSelected(i);
        }
    }

    EndScissorMode();
}

void UIManager::loadDefaultAsset(const std::string& path)
{
    loadAsset3D(path + "Block1.glb", "Dirt");
    loadAsset3D(path + "Block1.glb", "Dirt");
    loadAsset3D(path + "NewBlock.glb", "IDK YET");
}

void UIManager::loadAsset2D(const std::string& path, const std::string& name)
{
    Asset2D asset;
    asset.setFileName(path);
    asset.setDisplayName(name);
    asset.loadFile();  // assumes it loads the texture and sets _textureLoaded

    if (asset.isLoaded()) {
        _assetTiles2D.push_back(asset);
        std::cout << "Loaded 2D asset: " << name << " from " << path << std::endl;
    } else {
        std::cerr << "Failed to load 2D asset: " << name << " from " << path << std::endl;
    }
}


void UIManager::loadAsset3D(const std::string& path, const std::string& name)
{
    Asset3D asset;
    asset.setFileName(path);
    asset.setDisplayName(name);
    asset.loadFile();  // assumes it loads the model

    if (asset.isLoaded()) {
        _assetTiles3D.push_back(asset);
        std::cout << "Loaded 3D asset: " << name << " from " << path << std::endl;
    } else {
        std::cerr << "Failed to load 3D asset: " << name << " from " << path << std::endl;
    }
}

void UIManager::drawSubmenu(const char** items, int count, int x, int y, int width)
{
    // Background
    DrawRectangle(static_cast<int>(x), static_cast<int>(y), static_cast<int>(width), static_cast<int>(count * 30), UI_SECONDARY);
    DrawRectangleLinesEx({static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), count * 30.0f}, 1, UI_PRIMARY);
    
    // Draw items
    for (int i = 0; i < count; i++) {
        Rectangle itemBounds = {static_cast<float>(x), static_cast<float>(y + i * 30), static_cast<float>(width), 30.0f};
        
        // Check for hover
        bool hover = CheckCollisionPointRec(GetMousePosition(), itemBounds);
        
        // Draw hover background
        if (hover) {
            DrawRectangleRec(itemBounds, ACCENT_TERTIARY);
        }
        
        // Draw item text
        DrawText(items[i], static_cast<int>(x + 10), static_cast<int>(y + i * 30 + 10), 10, UI_TEXT_PRIMARY);
    }
}

void UIManager::loadIcons()
{
    // Create tool icons programmatically
    _toolIcons.resize(6);
    _toolIconRenderTextures.resize(6);
    
    const int iconSize = 32;
    const Color iconColor = WHITE;
    const Color bgColor = BLANK; // Transparent background
    
    // Generate each tool icon
    for (int i = 0; i < 6; i++) {
        // Create render texture for icon
        RenderTexture2D iconTexture = LoadRenderTexture(iconSize, iconSize);
        _toolIconRenderTextures[i] = iconTexture;
        
        BeginTextureMode(iconTexture);
        ClearBackground(bgColor);
        
        // Draw different icon based on tool type
        switch (i) {
            case 0: // SELECT tool - Arrow cursor
                {
                    Vector2 points[3] = {
                        {8, 6}, {8, 22}, {14, 16}
                    };
                    DrawTriangle(points[0], points[1], points[2], iconColor);
                    DrawTriangleLines(points[0], points[1], points[2], {180, 180, 180, 255});
                    // Add small selection box
                    DrawRectangleLines(18, 18, 8, 8, {150, 150, 255, 255});
                }
                break;
                
            case 1: // HAND tool - Hand/move icon
                {
                    // Draw directional arrows indicating movement
                    DrawTriangle({16, 6}, {12, 10}, {20, 10}, iconColor); // Up
                    DrawTriangle({16, 26}, {12, 22}, {20, 22}, iconColor); // Down
                    DrawTriangle({6, 16}, {10, 12}, {10, 20}, iconColor); // Left
                    DrawTriangle({26, 16}, {22, 12}, {22, 20}, iconColor); // Right
                    DrawCircle(16, 16, 3, {255, 200, 100, 255}); // Center point
                }
                break;
                
            case 2: // PEN tool - Pencil
                {
                    // Draw pencil body
                    DrawLine(8, 24, 20, 12, {255, 200, 100, 255}); // Wood body
                    DrawLine(9, 23, 21, 11, {255, 200, 100, 255});
                    DrawLine(20, 12, 24, 8, {100, 100, 100, 255}); // Metal ferrule
                    DrawLine(21, 11, 25, 7, {100, 100, 100, 255});
                    DrawCircle(6, 26, 2, {50, 50, 50, 255}); // Graphite tip
                    DrawCircle(26, 6, 2, {255, 100, 100, 255}); // Eraser
                }
                break;
                
            case 3: // ERASER tool - Eraser block
                {
                    DrawRectangle(10, 10, 12, 8, {255, 200, 200, 255}); // Pink eraser
                    DrawRectangleLines(10, 10, 12, 8, iconColor);
                    DrawRectangle(12, 18, 8, 4, {200, 150, 100, 255}); // Metal band
                    DrawRectangleLines(12, 18, 8, 4, iconColor);
                }
                break;
                
            case 4: // CUBE tool - 3D cube
                {
                    // Draw isometric cube
                    Vector2 cube_front[4] = {{8, 12}, {16, 12}, {16, 20}, {8, 20}};
                    Vector2 cube_top[4] = {{8, 12}, {12, 8}, {20, 8}, {16, 12}};
                    Vector2 cube_right[4] = {{16, 12}, {20, 8}, {20, 16}, {16, 20}};
                    
                    // Fill faces
                    DrawTriangleFan(cube_front, 4, {180, 180, 255, 255}); // Front face
                    DrawTriangleFan(cube_top, 4, {220, 220, 255, 255}); // Top face
                    DrawTriangleFan(cube_right, 4, {140, 140, 255, 255}); // Right face
                    
                    // Draw edges
                    for (int j = 0; j < 4; j++) {
                        DrawLineV(cube_front[j], cube_front[(j+1)%4], iconColor);
                        DrawLineV(cube_top[j], cube_top[(j+1)%4], iconColor);
                        DrawLineV(cube_right[j], cube_right[(j+1)%4], iconColor);
                    }
                }
                break;
                
            case 5: // ZOOM tool - Magnifying glass
                {
                    DrawCircleLines(12, 12, 6, iconColor);
                    DrawCircleLines(12, 12, 5, iconColor);
                    DrawLine(17, 17, 24, 24, iconColor);
                    DrawLine(16, 18, 23, 25, iconColor);
                    DrawText("+", 10, 10, 8, iconColor);
                }
                break;
        }
        
        EndTextureMode();
        
        // Store the texture
        _toolIcons[i] = iconTexture.texture;
    }
}

void UIManager::unloadIcons()
{
    // Properly unload render textures
    for (auto& renderTexture : _toolIconRenderTextures) {
        if (renderTexture.id > 0) {
            UnloadRenderTexture(renderTexture);
        }
    }
    _toolIconRenderTextures.clear();
    _toolIcons.clear();
}

void UIManager::togglePanel(PanelType panel)
{
    _panelVisibility[panel] = !_panelVisibility[panel];
}

bool UIManager::isPanelVisible(PanelType panel) const
{
    return _panelVisibility.at(panel);
}

void UIManager::setCurrentTool(ToolType tool)
{
    _currentTool = tool;
}

ToolType UIManager::getCurrentTool() const
{
    return _currentTool;
}

Rectangle UIManager::getMainViewArea() const
{
    return {
        static_cast<float>(_leftToolbarWidth),
        static_cast<float>(_topBarHeight),
        static_cast<float>(_screenWidth - _leftToolbarWidth - _rightPanelsWidth),
        static_cast<float>(_screenHeight - _topBarHeight - _bottomAssetsBarHeight)
    };
}

void UIManager::setupEventHandlers()
{
    // Subscribe to editor events to update UI state
    g_eventDispatcher.subscribe(EditorEventType::OBJECT_SELECTED, [this](const EditorEvent& event) {
        // Update selected object in scene panel
        if (std::holds_alternative<int>(event.data)) {
            int objectId = std::get<int>(event.data);
            // Find the object in the scene list and update selection
            for (int i = 0; i < _sceneObjects.size(); ++i) {
                if (_sceneObjects[i].id == objectId) {
                    _selectedObjectIndex = i;
                    // Update properties panel with selected object data
                    _position = _sceneObjects[i].position;
                    _rotation = _sceneObjects[i].rotation;
                    _scale = _sceneObjects[i].scale;
                    break;
                }
            }
        }
    });
    
    // Subscribe to scene update events
    g_eventDispatcher.subscribe(EditorEventType::SCENE_UPDATED, [this](const EditorEvent& event) {
        refreshSceneObjects();
    });
    
    g_eventDispatcher.subscribe(EditorEventType::SCENE_OBJECT_ADDED, [this](const EditorEvent& event) {
        refreshSceneObjects();
    });
    
    g_eventDispatcher.subscribe(EditorEventType::SCENE_OBJECT_REMOVED, [this](const EditorEvent& event) {
        refreshSceneObjects();
    });
    
    g_eventDispatcher.subscribe(EditorEventType::CAMERA_MOVED, [this](const EditorEvent& event) {
        // Could update camera info in UI if needed
    });
    
    g_eventDispatcher.subscribe(EditorEventType::ZOOM_CHANGED, [this](const EditorEvent& event) {
        // Could update zoom info in UI if needed
    });
}

void UIManager::handleFileMenuAction(int selectedItem)
{
    switch (selectedItem) {
        case 0: // New
            Events::fileAction(EditorEventType::FILE_NEW);
            break;
        case 1: // Open
            Events::fileAction(EditorEventType::FILE_OPEN);
            break;
        case 2: // Save
            Events::fileAction(EditorEventType::FILE_SAVE);
            break;
        case 3: // Export
            Events::fileAction(EditorEventType::FILE_EXPORT);
            break;
        case 4: // Exit
            // Handle exit
            break;
    }
}

void UIManager::handleEditMenuAction(int selectedItem)
{
    switch (selectedItem) {
        case 0: // Undo
            // Dispatch undo event
            break;
        case 1: // Redo
            // Dispatch redo event
            break;
        case 2: // Cut
            // Dispatch cut event
            break;
        case 3: // Copy
            // Dispatch copy event
            break;
        case 4: // Paste
            // Dispatch paste event
            break;
        case 5: // Delete
            if (_selectedObjectIndex >= 0) {
                Events::objectDeleted(_selectedObjectIndex);
            }
            break;
    }
}

void UIManager::handleRenderMenuAction(int selectedItem)
{
    switch (selectedItem) {
        case 0: // Preview
            // Toggle preview mode
            break;
        case 1: // Grid On/Off
            Events::gridToggled(true); // Would track actual state
            break;
        case 2: // Shadows
            // Toggle shadows
            break;
        case 3: // Quality
            // Adjust quality settings
            break;
    }
}

void UIManager::handleHelpMenuAction(int selectedItem)
{
    switch (selectedItem) {
        case 0: // Documentation
            // Open documentation
            break;
        case 1: // Shortcuts
            // Show shortcuts dialog
            break;
        case 2: // About
            // Show about dialog
            break;
    }
}

int UIManager::getSelectedObjectCount() const
{
    return _selectedObjectIndex >= 0 ? 1 : 0;
}

std::string UIManager::getSelectedObjectName() const
{
    if (_selectedObjectIndex >= 0 && _selectedObjectIndex < _sceneObjects.size()) {
        return _sceneObjects[_selectedObjectIndex].getDisplayName();
    }
    return "";
}

Vector3 UIManager::getSelectedObjectTransform() const
{
    return _position; // Return current transform values
}

void UIManager::updateSceneObjectsList(ISceneProvider* sceneProvider)
{
    if (sceneProvider) {
        _currentSceneProvider = sceneProvider;
        refreshSceneObjects();
    }
}

void UIManager::refreshSceneObjects()
{
    if (_currentSceneProvider) {
        _sceneObjects = _currentSceneProvider->getSceneObjects();
        
        // Update selected object index if needed
        int selectedObjectId = _currentSceneProvider->getSelectedObjectId();
        _selectedObjectIndex = -1;
        
        for (int i = 0; i < _sceneObjects.size(); ++i) {
            if (_sceneObjects[i].id == selectedObjectId) {
                _selectedObjectIndex = i;
                // Update properties panel with selected object data
                _position = _sceneObjects[i].position;
                _rotation = _sceneObjects[i].rotation;
                _scale = _sceneObjects[i].scale;
                break;
            }
        }
    }
}

} // namespace UI