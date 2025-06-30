/**
 * @file ScriptingEditor.hpp
 * @brief Scripting editor component for the IsoMaker game engine
 * @author IsoMaker Team
 * @version 0.1
 */

#pragma once

#include "raylib.h"
#include "raygui.h"
#include "Render/Window.hpp"
#include "Render/Camera.hpp"
#include "Input/MouseKeyboard.hpp"
#include "../../UI/EditorEvents.hpp"
#include "../../UI/SceneObject.hpp"
#include <vector>
#include <string>
#include <unordered_map>

/**
 * @brief Script template types for low-code scripting
 */
enum class ScriptTemplate {
    ROTATE,           ///< Rotate object around an axis
    MOVE,             ///< Move object along a vector
    TOGGLE_VISIBILITY, ///< Toggle object visibility
    SCALE,            ///< Scale object over time
    BLINK,            ///< Blink object on/off
    OSCILLATE         ///< Oscillate object position
};

/**
 * @brief Script configuration parameters
 */
struct ScriptConfig {
    ScriptTemplate templateType;
    std::unordered_map<std::string, float> floatParams;    ///< Float parameters (speed, duration, etc.)
    std::unordered_map<std::string, Vector3> vectorParams; ///< Vector parameters (axis, direction, etc.)
    std::unordered_map<std::string, bool> boolParams;      ///< Boolean parameters (loop, reverse, etc.)
    std::unordered_map<std::string, std::string> stringParams; ///< String parameters (name, etc.)
};

/**
 * @brief A single script attached to an object
 */
struct AttachedScript {
    int scriptId;           ///< Unique script ID
    std::string name;       ///< User-friendly script name
    ScriptConfig config;    ///< Script configuration
    bool enabled;           ///< Whether script is active
    
    AttachedScript(int id, const std::string& scriptName, ScriptTemplate templateType) 
        : scriptId(id), name(scriptName), enabled(true) {
        config.templateType = templateType;
    }
};

/**
 * @brief Scripting Editor class
 * 
 * The ScriptingEditor class provides a low-code scripting interface for the IsoMaker
 * game engine editor. It allows users to create and attach scripts to scene objects
 * through a visual interface.
 * 
 * Key features:
 * - Visual scripting interface (planned)
 * - Script attachment to scene objects (planned)  
 * - Event-driven communication with UI components
 * - Integration with existing editor architecture
 * 
 * Current state: Basic placeholder implementation with scaffolding for future development.
 * 
 * @see UI::EditorEvents For event communication
 * @see UI::ISceneProvider For scene data interface
 */
class ScriptingEditor : public UI::ISceneProvider {
public:
    /**
     * @brief Construct a new ScriptingEditor object
     */
    ScriptingEditor();

    /**
     * @brief Destroy the ScriptingEditor object
     * 
     * Cleanup is handled automatically by member destructors.
     */
    ~ScriptingEditor();

    /**
     * @brief Initialize the scripting editor
     * 
     * Initializes the scripting editor with camera and window references.
     * Sets up the event system and default editor state.
     * 
     * @param window Reference to the application window
     * @param camera Reference to the rendering camera
     */
    void init(std::shared_ptr<Render::Window> window, std::shared_ptr<Render::Camera> camera);

    /**
     * @brief Update the editor state
     * 
     * Updates the editor based on input and processes scripting-specific functionality.
     * Called once per frame.
     * 
     * @param inputHandler Reference to the input handler
     */
    void update(input::IHandlerBase &inputHandler);

    /**
     * @brief Draw all scripting editor elements
     * 
     * Renders the scripting editor interface within the specified view area.
     * 
     * @param mainViewArea Rectangle representing view area of scripting editor
     */
    void draw(Rectangle mainViewArea);

    // Event handling
    /**
     * @brief Set up event handlers for UI communication
     * 
     * Registers event handlers with the global event dispatcher for
     * receiving UI events like tool changes, file operations, etc.
     */
    void setupEventHandlers();

    /**
     * @brief Handle tool change events
     * 
     * Responds to tool change events from the UI and updates editor behavior.
     * 
     * @param toolIndex Index of the newly selected tool
     */
    void handleToolChanged(int toolIndex);

    /**
     * @brief Handle file operation events
     * 
     * Processes file-related events like save, load, new, etc.
     * 
     * @param actionType Type of file action to perform
     * @param filepath Optional file path for the operation
     */
    void handleFileAction(UI::EditorEventType actionType, const std::string& filepath = "");

    // State queries for UI
    /**
     * @brief Get the total number of scripts in the project
     * 
     * @return int Total count of scripts
     */
    int getScriptCount() const;

    /**
     * @brief Get the ID of the currently selected script
     * 
     * @return int ID of selected script, or -1 if none selected
     */
    int getSelectedObjectId() const override;

    /**
     * @brief Get the name of the currently selected script
     * 
     * @return std::string Name of the selected script, or empty if none
     */
    std::string getSelectedScriptName() const;
    
    /**
     * @brief Set the scene provider for object listing
     * 
     * @param provider Pointer to the scene provider
     */
    void setSceneProvider(UI::ISceneProvider* provider);

    // ISceneProvider interface implementation
    /**
     * @brief Get all scene objects for UI display
     * 
     * @return std::vector<UI::SceneObjectInfo> List of scene objects (empty for now)
     */
    std::vector<UI::SceneObjectInfo> getSceneObjects() const override;

    /**
     * @brief Get information about a specific object
     * 
     * @param objectId ID of the object to query
     * @return UI::SceneObjectInfo Object information structure
     */
    UI::SceneObjectInfo getObjectInfo(int objectId) const override;

    /**
     * @brief Select an object by ID
     * 
     * @param objectId ID of the object to select
     * @return true if selection successful, false otherwise
     */
    bool selectObject(int objectId) override;

    /**
     * @brief Delete an object by ID
     * 
     * @param objectId ID of the object to delete
     * @return true if deletion successful, false otherwise
     */
    bool deleteObject(int objectId) override;

private:
    // Core references
    std::shared_ptr<Render::Window> _window;             ///< Reference to the application window
    std::shared_ptr<Render::Camera> _camera;             ///< Reference to the 3D camera

    // Editor state
    int _currentTool = 0;                                ///< Current tool index
    int _selectedScriptId = -1;                          ///< ID of currently selected script
    int _selectedObjectId = -1;                          ///< Currently selected scene object
    
    // UI state
    bool _initialized = false;                           ///< Initialization state flag
    UI::ISceneProvider* _currentSceneProvider = nullptr; ///< Current scene provider for object list
    
    // Scripting data
    std::unordered_map<int, std::vector<AttachedScript>> _objectScripts; ///< Scripts per object ID
    int _nextScriptId = 1;                               ///< Next available script ID
    
    // UI layout constants
    const float _leftPanelWidth = 250.0f;               ///< Width of left objects panel
    const float _panelPadding = 10.0f;                  ///< Padding for panels
    
    // Add script dialog state
    bool _showAddScriptDialog = false;                   ///< Whether add script dialog is open
    int _selectedTemplateIndex = 0;                     ///< Selected template in dialog
    char _newScriptName[64] = "New Script";             ///< Name for new script
    
    // Script configuration state
    bool _showConfigDialog = false;                     ///< Whether config dialog is open
    AttachedScript* _editingScript = nullptr;           ///< Script being edited
    
    // UI helper methods
    void drawLeftPanel(Rectangle bounds);               ///< Draw left objects panel
    void drawMainPanel(Rectangle bounds);               ///< Draw main scripting panel
    void drawAddScriptDialog();                         ///< Draw add script dialog
    void drawConfigDialog();                            ///< Draw script configuration dialog
    void drawScriptList(int objectId, Rectangle bounds); ///< Draw list of scripts for object
    
    // Script management
    void addScriptToObject(int objectId, ScriptTemplate templateType, const std::string& name);
    void removeScriptFromObject(int objectId, int scriptId);
    std::vector<AttachedScript>& getObjectScripts(int objectId);
    std::string getTemplateDisplayName(ScriptTemplate templateType) const;
    std::string generatePseudocode(const AttachedScript& script) const;
    
    // UI interaction
    void handleObjectSelection(int objectId);
    void openAddScriptDialog();
    void closeAddScriptDialog();
    void openConfigDialog(AttachedScript* script);
    void closeConfigDialog();
};