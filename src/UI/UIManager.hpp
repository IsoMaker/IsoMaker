#pragma once
/**
 * @file UIManager.hpp
 * @brief User interface management system for the IsoMaker game engine editor
 * @author IsoMaker Team
 * @version 0.1
 */

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

/**
 * @brief Tool types available in the editor
 * 
 * Enumeration of all tools that can be selected in the left toolbar.
 * Each tool provides different functionality for editing and navigation.
 */
enum class ToolType {
    SELECT, ///< Selection tool for picking and manipulating objects
    HAND,   ///< Hand tool for panning the view
    PEN,    ///< Pen tool for drawing/painting operations
    ERASER, ///< Eraser tool for removing elements
    CUBE,   ///< Cube placement tool for adding 3D objects
    ZOOM    ///< Zoom tool for view scaling
};

// std::ostream& operator<<(std::ostream &os, const ToolType &color) {
//     switch (color) {
//         case ToolType::SELECT:
//             os << "Select";
//             break;
//         case ToolType::HAND:
//             os << "Hand";
//             break;
//         case ToolType::PEN:
//             os << "Pen";
//             break;
//             case ToolType::ERASER:
//             os << "Eraser";
//             break;
//         case ToolType::CUBE:
//             os << "Cube";
//             break;
//         case ToolType::ZOOM:
//             os << "Zoom";
//             break;
//         default:
//             os << "Tool";
//             break;
//     }
//     return os;
// }

/**
 * @brief Panel types in the right sidebar
 * 
 * Enumeration of panels that can be shown/hidden in the right sidebar area.
 */
enum class PanelType {
    SCENE,      ///< Scene hierarchy panel showing object tree
    PROPERTIES  ///< Properties panel showing selected object details
};

/**
 * @brief User Interface Manager class
 * 
 * The UIManager class handles all user interface rendering and interaction for the IsoMaker
 * game engine editor. It manages the layout and functionality of all UI components including
 * the top menu bar, left toolbar, right panels, and bottom assets browser.
 * 
 * Key responsibilities:
 * - UI layout and rendering management
 * - Event-driven communication with editors
 * - Tool selection and state management
 * - Panel visibility management
 * - Asset browser functionality
 * - Scene object hierarchy display
 * - Properties panel management
 * 
 * The UI is organized into several main areas:
 * - Top menu bar: File, Edit, Render, Help menus
 * - Left toolbar: Tool selection (Select, Hand, Pen, Eraser, Cube, Zoom)
 * - Right panels: Scene hierarchy and Properties panels
 * - Bottom bar: Assets browser
 * - Center area: Main viewport for editing
 * 
 * @see EditorEvents For event-driven communication system
 * @see ToolType For available tool types
 * @see PanelType For panel management
 */
class UIManager {
public:
    /**
     * @brief Construct a new UIManager object
     * 
     * Initializes the UI manager with the specified screen dimensions.
     * Sets up default UI state and prepares all UI components.
     * 
     * @param screenWidth Width of the application window
     * @param screenHeight Height of the application window
     */
    UIManager(int screenWidth, int screenHeight);
    
    /**
     * @brief Destroy the UIManager object
     * 
     * Cleans up UI resources, unloads textures, and handles proper cleanup.
     */
    ~UIManager();

    /**
     * @brief Initialize the UI system
     * 
     * Sets up event handlers, loads UI resources (icons, textures),
     * and prepares the UI for operation.
     */
    void initialize();
    
    /**
     * @brief Update UI state based on input
     * 
     * Processes input events, updates UI state, handles menu interactions,
     * and dispatches events to appropriate handlers.
     * 
     * @param inputHandler Reference to input handler for processing user input
     */
    void update(input::IHandlerBase &inputHandler);
    
    /**
     * @brief Render all UI components
     * 
     * Draws all UI elements including menus, toolbars, panels, and the assets browser.
     * Should be called every frame after the main viewport rendering.
     * 
     * @param mapEditor Reference to the map editor for querying editor state
     */
    void draw(MapEditor &mapEditor);

    // UI Component functions
    /**
     * @brief Draw the top menu bar
     * 
     * Renders the top menu bar containing File, Edit, Render, and Help menus.
     * Handles menu interaction and dispatches appropriate events.
     */
    void drawTopMenuBar();
    
    /**
     * @brief Draw the left toolbar
     * 
     * Renders the left toolbar with tool selection buttons.
     * Handles tool selection and visual feedback for the current tool.
     */
    void drawLeftToolbar();
    
    /**
     * @brief Draw the right panels
     * 
     * Renders the right sidebar panels including Scene hierarchy and Properties.
     * Handles panel content, interactions, and resizing.
     */
    void drawRightPanels();
    
    /**
     * @brief Draw the bottom assets bar
     * 
     * Renders the bottom assets browser showing available assets.
     * Handles asset selection and loading.
     */
    void drawBottomAssetsBar();
    
    // Panel management
    /**
     * @brief Toggle visibility of a panel
     * 
     * Shows or hides the specified panel type.
     * 
     * @param panel The panel type to toggle
     */
    void togglePanel(PanelType panel);
    
    /**
     * @brief Check if a panel is visible
     * 
     * @param panel The panel type to check
     * @return true if the panel is visible, false otherwise
     */
    bool isPanelVisible(PanelType panel) const;
    
    // Tool selection
    /**
     * @brief Set the current active tool
     * 
     * Changes the active tool and dispatches a TOOL_CHANGED event.
     * 
     * @param tool The tool type to activate
     */
    void setCurrentTool(ToolType tool);
    
    /**
     * @brief Get the currently active tool
     * 
     * @return ToolType The currently active tool
     */
    ToolType getCurrentTool() const;
    
    // Get UI regions
    /**
     * @brief Get the main viewport area
     * 
     * Calculates and returns the rectangle representing the main editing viewport,
     * excluding UI panels and toolbars.
     * 
     * @return Rectangle The main viewport area
     */
    Rectangle getMainViewArea() const;
    
    // Event handling
    /**
     * @brief Set up event handlers for UI-editor communication
     * 
     * Registers event handlers with the global event dispatcher to enable
     * communication between UI components and editors.
     */
    void setupEventHandlers();
    
    /**
     * @brief Handle File menu actions
     * 
     * Processes File menu selections and dispatches appropriate events.
     * 
     * @param selectedItem Index of the selected menu item
     */
    void handleFileMenuAction(int selectedItem);
    
    /**
     * @brief Handle Edit menu actions
     * 
     * Processes Edit menu selections and dispatches appropriate events.
     * 
     * @param selectedItem Index of the selected menu item
     */
    void handleEditMenuAction(int selectedItem);
    
    /**
     * @brief Handle Render menu actions
     * 
     * Processes Render menu selections and dispatches appropriate events.
     * 
     * @param selectedItem Index of the selected menu item
     */
    void handleRenderMenuAction(int selectedItem);
    
    /**
     * @brief Handle Help menu actions
     * 
     * Processes Help menu selections and dispatches appropriate events.
     * 
     * @param selectedItem Index of the selected menu item
     */
    void handleHelpMenuAction(int selectedItem);
    
    // Editor state queries
    /**
     * @brief Get the number of selected objects
     * 
     * @return int Number of currently selected objects in the editor
     */
    int getSelectedObjectCount() const;
    
    /**
     * @brief Get the name of the selected object
     * 
     * @return std::string Name of the currently selected object, or empty string if none
     */
    std::string getSelectedObjectName() const;
    
    /**
     * @brief Get the transform of the selected object
     * 
     * @return Vector3 Position vector of the currently selected object
     */
    Vector3 getSelectedObjectTransform() const;
    
    // Scene management
    /**
     * @brief Update the scene objects list
     * 
     * Updates the scene hierarchy panel with objects from the provided scene provider.
     * 
     * @param sceneProvider Pointer to the scene provider interface
     */
    void updateSceneObjectsList(ISceneProvider* sceneProvider);
    
    /**
     * @brief Refresh the scene objects display
     * 
     * Refreshes the scene hierarchy panel to reflect current scene state.
     */
    void refreshSceneObjects();
    
private:
    // Window dimensions
    int _screenWidth;                      ///< Application window width
    int _screenHeight;                     ///< Application window height
    
    // UI dimensions
    const int _topBarHeight = 30;          ///< Height of the top menu bar
    const int _leftToolbarWidth = 60;      ///< Width of the left toolbar
    const int _rightPanelsWidth = 260;     ///< Width of the right panels area
    const int _bottomAssetsBarHeight = 140; ///< Height of the bottom assets bar
    
    // UI state tracking
    ToolType _currentTool;                 ///< Currently selected tool
    std::unordered_map<PanelType, bool> _panelVisibility; ///< Panel visibility states
    
    // Menu state
    bool _fileMenuOpen;                    ///< File menu open state
    bool _editMenuOpen;                    ///< Edit menu open state
    bool _renderMenuOpen;                  ///< Render menu open state
    bool _helpMenuOpen;                    ///< Help menu open state
    
    // Tool icons
    std::vector<Texture2D> _toolIcons;     ///< Tool icon textures
    std::vector<RenderTexture2D> _toolIconRenderTextures; ///< Tool icon render textures
    
    // Assets for the bottom bar
    std::vector<Asset2D> _assetTiles;      ///< Available 2D assets for the bottom bar
    int _selectedAssetIndex;               ///< Index of currently selected asset

    // Scene objects
    std::vector<SceneObjectInfo> _sceneObjects; ///< List of scene objects for hierarchy
    int _selectedObjectIndex;              ///< Index of currently selected scene object
    ISceneProvider* _currentSceneProvider; ///< Current scene provider interface
    
    // Property sections
    bool _transformSectionOpen;            ///< Transform properties section open state
    bool _lightingSectionOpen;             ///< Lighting properties section open state
    bool _physicsSectionOpen;              ///< Physics properties section open state
    
    // Property values (example)
    Vector3 _position;                     ///< Current object position for properties panel
    Vector3 _rotation;                     ///< Current object rotation for properties panel
    Vector3 _scale;                        ///< Current object scale for properties panel
    
    // Search functionality
    char _searchText[64];                  ///< Search text buffer
    bool _searchActive;                    ///< Search active state
    
    // Current editor type
    int _currentEditorType;                ///< Current editor type (2D/3D)
    
    // UI helper functions
    /**
     * @brief Draw a submenu
     * 
     * Helper function to draw dropdown submenus.
     * 
     * @param items Array of menu item strings
     * @param count Number of menu items
     * @param x X position of the submenu
     * @param y Y position of the submenu
     * @param width Width of the submenu
     */
    void drawSubmenu(const char** items, int count, int x, int y, int width);
    
    /**
     * @brief Load UI icons
     * 
     * Loads all necessary UI icons and textures.
     */
    void loadIcons();
    
    /**
     * @brief Unload UI icons
     * 
     * Unloads and frees UI icon textures.
     */
    void unloadIcons();
};

} // namespace UI
