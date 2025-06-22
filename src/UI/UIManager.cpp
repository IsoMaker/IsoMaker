#include "UIManager.hpp"
#include <iostream>

namespace UI {

UIManager::UIManager(int screenWidth, int screenHeight)
    : _screenWidth(screenWidth),
      _screenHeight(screenHeight),
      _currentTool(ToolType::SELECT),
      _selectedAssetIndex(-1),
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
    drawTopMenuBar();
    drawLeftToolbar();
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
    const char *editorTypes[2] = { "Map Editor", "2D Editor" };
    
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
        
        // Create a placeholder icon texture (empty)
        Texture2D placeholderIcon = { 0 }; // Would be actual icon in production
        
        if (ToolButton(toolBounds, placeholderIcon, toolTips[i], static_cast<int>(_currentTool) == i)) {
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
        
        // Use CollapsiblePanel component
        CollapsiblePanel(lightingBounds, "Lighting", &_lightingSectionOpen, UI_PRIMARY, UI_SECONDARY);
        
        if (_lightingSectionOpen) {
            // Lighting properties would go here
            sectionY += 80;
        } else {
            sectionY += sectionHeight + 5;
        }
        
        // Physics section
        Rectangle physicsBounds = {
            static_cast<float>(_screenWidth - _rightPanelsWidth + 5),
            static_cast<float>(sectionY),
            static_cast<float>(_rightPanelsWidth - 10),
            static_cast<float>(sectionHeight)
        };
        
        // Use CollapsiblePanel component
        CollapsiblePanel(physicsBounds, "Physics", &_physicsSectionOpen, UI_PRIMARY, UI_SECONDARY);
        
        if (_physicsSectionOpen) {
            // Physics properties would go here
            sectionY += 80;
        } else {
            sectionY += sectionHeight + 5;
        }
    }
}

void UIManager::drawBottomAssetsBar()
{
    int barY = _screenHeight - _bottomAssetsBarHeight;
    
    // Draw assets bar background
    DrawRectangle(0, static_cast<int>(barY), static_cast<int>(_screenWidth), static_cast<int>(_bottomAssetsBarHeight), UI_SECONDARY);
    
    // Draw header
    DrawRectangle(0, static_cast<int>(barY), static_cast<int>(_screenWidth), 30, UI_PRIMARY);
    DrawText("Assets", 10, static_cast<int>(barY + 10), 10, UI_TEXT_PRIMARY);
    
    // Draw asset grid (placeholders)
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
        
        if (AssetTile(assetBounds, emptyTexture, assetName, i == _selectedAssetIndex)) {
            _selectedAssetIndex = i;
            Events::assetSelected(i);
        }
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
    // This would load actual icons
    // Example:
    // _toolIcons.resize(6);
    // _toolIcons[0] = LoadTexture("path/to/select_icon.png");
    // ...
}

void UIManager::unloadIcons()
{
    // Unload textures when done
    for (auto& icon : _toolIcons) {
        UnloadTexture(icon);
    }
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