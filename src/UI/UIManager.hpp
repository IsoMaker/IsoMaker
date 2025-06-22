#pragma once

#include "raylib.h"
#include "raygui.h"
#include "UITheme.hpp"
#include "UIComponents.hpp"
#include "EditorEvents.hpp"
#include "SceneObject.hpp"
#include "Input/MouseKeyboard.hpp"
#include "../Editor/3DMap/3DMapEditor.hpp"
#include "Assets/Asset2D.hpp"
#include "Assets/Asset3D.hpp"
#include <string>
#include <vector>
#include <unordered_map>

namespace UI {

enum class ToolType {
    SELECT,
    HAND,
    PEN,
    ERASER,
    CUBE,
    ZOOM
};

enum class PanelType {
    SCENE,
    PROPERTIES
};

class UIManager {
public:
    UIManager(int screenWidth, int screenHeight);
    ~UIManager();

    void initialize();
    void update(input::IHandlerBase &inputHandler);
    void draw(MapEditor &mapEditor);

    // UI Component functions
    void drawTopMenuBar();
    void drawLeftToolbar();
    void drawRightPanels();
    void drawBottomAssetsBar();
    
    // Panel management
    void togglePanel(PanelType panel);
    bool isPanelVisible(PanelType panel) const;
    
    // Tool selection
    void setCurrentTool(ToolType tool);
    ToolType getCurrentTool() const;
    
    // Get UI regions
    Rectangle getMainViewArea() const;
    
    // Event handling
    void setupEventHandlers();
    void handleFileMenuAction(int selectedItem);
    void handleEditMenuAction(int selectedItem);
    void handleRenderMenuAction(int selectedItem);
    void handleHelpMenuAction(int selectedItem);
    
    // Editor state queries
    int getSelectedObjectCount() const;
    std::string getSelectedObjectName() const;
    Vector3 getSelectedObjectTransform() const;
    
    // Scene management
    void updateSceneObjectsList(ISceneProvider* sceneProvider);
    void refreshSceneObjects();
    
private:
    // Window dimensions
    int _screenWidth;
    int _screenHeight;
    
    // UI dimensions
    const int _topBarHeight = 30;
    const int _leftToolbarWidth = 60;
    const int _rightPanelsWidth = 260;
    const int _bottomAssetsBarHeight = 140;
    
    // UI state tracking
    ToolType _currentTool;
    std::unordered_map<PanelType, bool> _panelVisibility;
    
    // Menu state
    bool _fileMenuOpen;
    bool _editMenuOpen;
    bool _renderMenuOpen;
    bool _helpMenuOpen;
    
    // Tool icons
    std::vector<Texture2D> _toolIcons;
    std::vector<RenderTexture2D> _toolIconRenderTextures;
    
    // Assets for the bottom bar
    std::vector<Asset2D> _assetTiles;
    int _selectedAssetIndex;

    // Scene objects
    std::vector<SceneObjectInfo> _sceneObjects;
    int _selectedObjectIndex;
    ISceneProvider* _currentSceneProvider;
    
    // Property sections
    bool _transformSectionOpen;
    bool _lightingSectionOpen;
    bool _physicsSectionOpen;
    
    // Property values (example)
    Vector3 _position;
    Vector3 _rotation;
    Vector3 _scale;
    
    // Search functionality
    char _searchText[64];
    bool _searchActive;
    
    // Current editor type
    int _currentEditorType;
    
    // UI helper functions
    void drawSubmenu(const char** items, int count, int x, int y, int width);
    void loadIcons();
    void unloadIcons();
};

} // namespace UI