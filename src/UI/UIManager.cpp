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
      _editorMenuOpen(false),
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
    // Setup event handlers
    setupEventHandlers();
}

void UIManager::update(input::IHandlerBase &inputHandler)
{
    Vector2 mousePos = { static_cast<float>(inputHandler.getCursorCoords().x), 
                         static_cast<float>(inputHandler.getCursorCoords().y) };
    
    // Check for clicks outside menus to close them
    if (inputHandler.isPressed(input::Generic::SELECT1)) {
        // Close menus if clicked outside - but not if clicking on submenu items
        bool clickedOnMenu = false;
        
        // Check if clicking on any submenu area
        if (_fileMenuOpen) {
            Rectangle fileSubmenuArea = {10.0f, 30.0f, 120.0f, 125.0f};
            if (CheckCollisionPointRec(mousePos, fileSubmenuArea)) clickedOnMenu = true;
        }
        if (_editMenuOpen) {
            Rectangle editSubmenuArea = {90.0f, 30.0f, 120.0f, 150.0f};
            if (CheckCollisionPointRec(mousePos, editSubmenuArea)) clickedOnMenu = true;
        }
        if (_renderMenuOpen) {
            Rectangle renderSubmenuArea = {170.0f, 30.0f, 120.0f, 100.0f};
            if (CheckCollisionPointRec(mousePos, renderSubmenuArea)) clickedOnMenu = true;
        }
        if (_helpMenuOpen) {
            Rectangle helpSubmenuArea = {250.0f, 30.0f, 120.0f, 75.0f};
            if (CheckCollisionPointRec(mousePos, helpSubmenuArea)) clickedOnMenu = true;
        }
        if (_editorMenuOpen) {
            Rectangle editorSubmenuArea = {static_cast<float>(_screenWidth - 80 - 10), 30.0f, 120.0f, 60.0f};
            if (CheckCollisionPointRec(mousePos, editorSubmenuArea)) clickedOnMenu = true;
        }
        
        // Only close menus if not clicking on submenu items
        if (!clickedOnMenu && mousePos.y > 30) {
            _fileMenuOpen = false;
            _editMenuOpen = false;
            _renderMenuOpen = false;
            _helpMenuOpen = false;
            _editorMenuOpen = false;
        }
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
    drawRightPanels();
	drawTopMenuBar();
    drawBottomAssetsBar();
    
}

void UIManager::draw(ISceneProvider &sceneProvider)
{
    // Update scene provider reference
    if (_currentSceneProvider != &sceneProvider) {
        _currentSceneProvider = &sceneProvider;
        refreshSceneObjects();
    }
    
    // Draw UI components - only top menu bar for scripting editor
    drawTopMenuBar();
}

void UIManager::drawTopMenuBar()
{
    // Draw top bar background with shadow
    Rectangle topBarBounds = {0, 0, static_cast<float>(_screenWidth), static_cast<float>(_topBarHeight)};
    DrawRectangleRoundedWithShadow(topBarBounds, 0, UI_PRIMARY, UI_SHADOW_OFFSET_SMALL, SHADOW_MEDIUM);
    
    // Menu items
    int menuItemWidth = 80;
    
    // File menu
    Rectangle fileRect = {10.0f, 0.0f, static_cast<float>(menuItemWidth), static_cast<float>(_topBarHeight)};
    if (MenuItem(fileRect, "File", &_fileMenuOpen)) {
        _editMenuOpen = false;
        _renderMenuOpen = false;
        _helpMenuOpen = false;
        _editorMenuOpen = false;
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
        _editorMenuOpen = false;
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
        _editorMenuOpen = false;
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
        _editorMenuOpen = false;
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
    
    // Current editor label (centered) with enhanced styling
    const char* editorNames[2] = {"Map Editor", "Scripting Editor"};
    const char* currentEditorName = editorNames[_currentEditorType];
    int textWidth = MeasureText(currentEditorName, UI_FONT_SIZE_XLARGE);
    int centerX = _screenWidth / 2 - textWidth / 2;
    
    // Draw subtle background for editor label
    Rectangle labelBounds = {
        static_cast<float>(centerX - UI_PADDING_MEDIUM), 
        2, 
        static_cast<float>(textWidth + 2 * UI_PADDING_MEDIUM), 
        static_cast<float>(_topBarHeight - 4)
    };
    DrawRectangleRounded(labelBounds, UI_BORDER_RADIUS_SMALL, UI_SECONDARY);
    
    DrawText(currentEditorName, centerX, 7, UI_FONT_SIZE_XLARGE, UI_TEXT_PRIMARY);
    
    // Editor menu (right aligned)
    Rectangle editorRect = {static_cast<float>(_screenWidth - menuItemWidth - 10), 0.0f, static_cast<float>(menuItemWidth), static_cast<float>(_topBarHeight)};
    if (MenuItem(editorRect, "Editor", &_editorMenuOpen)) {
        _fileMenuOpen = false;
        _editMenuOpen = false;
        _renderMenuOpen = false;
        _helpMenuOpen = false;
    }
    
    // Editor submenu
    if (_editorMenuOpen) {
        const char* editorItems[2] = {"Map Editor", "Scripting Editor"};
        int editorItemSelected = Submenu({editorRect.x, editorRect.y + editorRect.height, 120.0f, 60.0f}, editorItems, 2);
        
        // Handle editor menu selection
        if (editorItemSelected >= 0) {
            _editorMenuOpen = false;
            handleEditorMenuAction(editorItemSelected);
        }
    }
}

void UIManager::drawLeftToolbar()
{
    // Draw toolbar background with enhanced styling
    Rectangle toolbarBounds = {
        0, 
        static_cast<float>(_topBarHeight), 
        static_cast<float>(_leftToolbarWidth), 
        static_cast<float>(_screenHeight - _topBarHeight - _bottomAssetsBarHeight)
    };
    DrawRectangleRoundedWithShadow(toolbarBounds, 0, PANEL_BACKGROUND, UI_SHADOW_OFFSET_SMALL, SHADOW_LIGHT);
    
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
    
    // Scene panel with enhanced styling
    if (_panelVisibility[PanelType::SCENE]) {
        // Panel bounds with padding
        Rectangle panelBounds = {
            static_cast<float>(_screenWidth - _rightPanelsWidth),
            static_cast<float>(panelY),
            static_cast<float>(_rightPanelsWidth),
            static_cast<float>(panelHeight)
        };
        
        // Draw panel with shadow and rounded corners
        DrawPanel(panelBounds, "Scene", PANEL_HEADER, PANEL_BACKGROUND, true);
        
        // Use the CollapsiblePanel component - though always open for scene
        bool alwaysOpen = true;
        CollapsiblePanel(panelBounds, "Scene", &alwaysOpen, PANEL_HEADER, PANEL_BACKGROUND);
        
        // Scene objects list with enhanced styling
        int itemStartY = panelY + UI_PADDING_XLARGE + UI_PADDING_LARGE;
        for (int i = 0; i < _sceneObjects.size(); i++) {
            Rectangle itemBounds = {
                static_cast<float>(_screenWidth - _rightPanelsWidth + UI_PADDING_MEDIUM),
                static_cast<float>(itemStartY + i * (UI_PADDING_XLARGE + UI_PADDING_SMALL)),
                static_cast<float>(_rightPanelsWidth - 2 * UI_PADDING_MEDIUM),
                UI_PADDING_XLARGE
            };
            
            const SceneObjectInfo& obj = _sceneObjects[i];
            bool isHovered = CheckCollisionPointRec(GetMousePosition(), itemBounds);
            
            // Enhanced background for selected/hovered items
            if (obj.isSelected || i == _selectedObjectIndex) {
                DrawRectangleRounded(itemBounds, UI_BORDER_RADIUS_SMALL, SELECTED_BACKGROUND);
            } else if (isHovered) {
                DrawRectangleRounded(itemBounds, UI_BORDER_RADIUS_SMALL, HOVER_BACKGROUND);
            }
            
            // Enhanced type indicator colors
            Color typeColor = UI_TEXT_PRIMARY;
            switch (obj.type) {
                case SceneObjectType::CUBE_3D: typeColor = ACCENT_PRIMARY; break;
                case SceneObjectType::SPRITE_2D: typeColor = ACCENT_SECONDARY; break;
                case SceneObjectType::CAMERA: typeColor = ACCENT_TERTIARY; break;
                case SceneObjectType::LIGHT: typeColor = WARNING; break;
                default: typeColor = UI_TEXT_SECONDARY; break;
            }
            
            // Draw visibility indicator with better spacing
            const char* visibilityIcon = obj.isVisible ? "●" : "○";
            DrawText(visibilityIcon, itemBounds.x + UI_PADDING_SMALL, itemBounds.y + UI_PADDING_SMALL, UI_FONT_SIZE_MEDIUM, typeColor);
            
            // Draw object name with better typography
            DrawText(obj.getDisplayName().c_str(), 
                    itemBounds.x + UI_PADDING_XLARGE, 
                    itemBounds.y + UI_PADDING_SMALL, 
                    UI_FONT_SIZE_MEDIUM, 
                    UI_TEXT_PRIMARY);
            
            // Check for selection
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && isHovered) {
                _selectedObjectIndex = i;
                if (_currentSceneProvider) {
                    _currentSceneProvider->selectObject(obj.id);
                }
                Events::objectSelected(obj.id);
            }
        }
    }
    
    // Properties panel with enhanced styling
    if (_panelVisibility[PanelType::PROPERTIES]) {
        panelY += panelHeight;
        
        // Enhanced panel bounds
        Rectangle propertiesBounds = {
            static_cast<float>(_screenWidth - _rightPanelsWidth),
            static_cast<float>(panelY),
            static_cast<float>(_rightPanelsWidth),
            static_cast<float>(panelHeight)
        };
        
        // Draw panel with shadow and rounded corners
        DrawPanel(propertiesBounds, "Properties", PANEL_HEADER, PANEL_BACKGROUND, true);
        
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
    
    // Enhanced assets bar with shadow and rounded corners
    Rectangle assetsBounds = {
        0, 
        static_cast<float>(barY), 
        static_cast<float>(_screenWidth), 
        static_cast<float>(_bottomAssetsBarHeight)
    };
    DrawPanel(assetsBounds, "Assets", PANEL_HEADER, PANEL_BACKGROUND, true);
    
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
    DrawText("+", 150, static_cast<int>(barY + 10), 10, buttonColorPlus);
    
    if (CheckCollisionPointRec(GetMousePosition(), buttonRectPlus) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        std::cout << "+ button clicked! Launching asset preview window." << std::endl;
        openAssetWindow();
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
    std::vector<Asset2D> assetTiles2D = _loader->getLoaded2DAssets();

    for (int i = 0; i < assetTiles2D.size(); i++) {
        int row = i / rowCapacity;
        int col = i % rowCapacity;

        int x = padding + col * (assetSize + padding);
        int y = barY + 40 + row * (assetSize + padding);

        Rectangle assetBounds = {static_cast<float>(x), static_cast<float>(y), static_cast<float>(assetSize), static_cast<float>(assetSize)};
        Rectangle tileBounds = { (float)x, (float)y, (float)assetSize, (float)assetSize };

        Asset2D asset = assetTiles2D[i];
        Texture2D texture = asset.getTexture();

        if (AssetTile(assetBounds, asset, i == _selectedAssetIndex2D, {x, y})) {
            _selectedAssetIndex2D = i;
            std::shared_ptr<Asset2D> assetBasic = std::make_shared<Asset2D>(assetTiles2D[i]);
            Events::assetSelected(assetBasic);
        }

        if (asset.isLoaded()) {

        }

        // Selection
        if (CheckCollisionPointRec(GetMousePosition(), tileBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            _selectedAssetIndex2D = i;
            std::shared_ptr<Asset2D> assetBasic = std::make_shared<Asset2D>(assetTiles2D[i]);
            Events::assetSelected(assetBasic);
        }
    }
}

void UIManager::drawModelPreview(Asset3D asset, Rectangle assetBounds, int barY)
{
    // Adjust bounds to leave space for text at bottom and center the 3D preview
    Rectangle previewBounds = {
        assetBounds.x + 5, 
        assetBounds.y + 5, 
        assetBounds.width - 10, 
        assetBounds.height - 25  // Leave 20px for text plus some padding
    };
    
    BeginScissorMode((int)previewBounds.x, (int)previewBounds.y, (int)previewBounds.width, (int)previewBounds.height);
    rlViewport(previewBounds.x, GetScreenHeight() - (previewBounds.y + previewBounds.height), previewBounds.width, previewBounds.height);

    Camera cam = { 0 };
    cam.position = { 2.0f, 2.0f, 2.0f };
    cam.target = { 0.0f, 0.0f, 0.0f };  // Center the target
    cam.up = { 0.0f, 1.0f, 0.0f };
    cam.fovy = 45.0f;
    cam.projection = CAMERA_PERSPECTIVE;

    BeginMode3D(cam);

    // Center the model and make it appropriately sized for the preview
    Vector3 pos = { 0.0f, 0.0f, 0.0f };  // Center the model
    Vector3 rotAxis = { 0.0f, 1.0f, 0.0f };
    float angle = GetTime() * 45.0f;
    
    // Scale the model appropriately for the preview size
    float previewScale = asset.getScale() * 0.8f;  // Slightly smaller to fit nicely
    DrawModelEx(asset.getModel(), pos, rotAxis, angle, {previewScale, previewScale, previewScale}, WHITE);

    EndMode3D();

    rlViewport(0, 0, GetScreenWidth(), GetScreenHeight());
    rlLoadIdentity();
    EndScissorMode();
}

void UIManager::drawBottomAssets3D(int barY)
{
    int assetSize = 80;
    int padding = 10;
    int rowCapacity = (_screenWidth - padding) / (assetSize + padding);
    std::vector<Asset3D> assetTiles3D = _loader->getLoaded3DAssets();

    for (int i = 0; i < assetTiles3D.size(); i++) {
        int row = i / rowCapacity;
        int col = i % rowCapacity;

        int x = padding + col * (assetSize + padding);
        int y = barY + 40 + row * (assetSize + padding);

        Rectangle assetBounds = {static_cast<float>(x), static_cast<float>(y), static_cast<float>(assetSize), static_cast<float>(assetSize)};
        Rectangle tileBounds = { (float)x, (float)y, (float)assetSize, (float)assetSize };


        if (AssetTile(assetBounds, assetTiles3D[i].getModel(), assetTiles3D[i].getDisplayName().c_str(), i == _selectedAssetIndex3D)) {
            _selectedAssetIndex3D = i;
            std::shared_ptr<Asset3D> assetBasic = std::make_shared<Asset3D>(assetTiles3D[i]);
            Events::assetSelected(assetBasic);
        }

        drawModelPreview(assetTiles3D[i], assetBounds, barY);

        if (CheckCollisionPointRec(GetMousePosition(), tileBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            _selectedAssetIndex3D = i;
            std::shared_ptr<Asset3D> assetBasic = std::make_shared<Asset3D>(assetTiles3D[i]);
            Events::assetSelected(assetBasic);
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

Rectangle UIManager::getFullViewArea() const
{
    return {
        0.0f,
        static_cast<float>(_topBarHeight),
        static_cast<float>(_screenWidth),
        static_cast<float>(_screenHeight - _topBarHeight)
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

void UIManager::handleEditorMenuAction(int selectedItem)
{
    switch (selectedItem) {
        case 0: // Map Editor
            _currentEditorType = 0;
            Events::editorModeChanged(_currentEditorType);
            break;
        case 1: // Scripting Editor
            _currentEditorType = 1;
            Events::editorModeChanged(_currentEditorType);
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

void UIManager::openAssetWindow() {
    pid_t pid = fork();
    if (pid == 0) {
        execl("bin/OpenAsset", "AssetWindow", nullptr);
        _exit(1);
    } else if (pid < 0) {
        std::cerr << "Failed to fork AssetWindow\n";
    }
}

void UIManager::setLoader(std::shared_ptr<AssetLoader> loader)
{
    _loader = loader;
}

} // namespace UI
