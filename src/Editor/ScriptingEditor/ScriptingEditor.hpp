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
 * @brief Block categories for the visual scripting system
 */
enum class BlockCategory {
    EVENTS,      ///< Event blocks (OnStart, OnClick, OnUpdate)
    ACTIONS,     ///< Action blocks (Move, Rotate, Change Color, etc.)
    CONDITIONS,  ///< Condition blocks (If, If Else, While)
    MISC         ///< Miscellaneous blocks (True, False, Value, etc.)
};

/**
 * @brief Block types for visual scripting
 */
enum class BlockType {
    // Special
    INVALID,      ///< Invalid/not found marker
    
    // Events
    ON_START,
    ON_CLICK,
    ON_UPDATE,
    ON_KEY_PRESS,
    
    // Actions
    MOVE,
    ROTATE,
    CHANGE_COLOR,
    HIDE,
    SHOW,
    
    // Conditions
    IF,
    IF_ELSE,
    WHILE,
    
    // Miscellaneous
    TRUE,
    FALSE,
    VALUE,
    ENTITY,
    DELAY,
    LOG
};

/**
 * @brief Block configuration parameters
 */
struct BlockConfig {
    std::unordered_map<std::string, float> floatParams;     ///< Float parameters (speed, distance, etc.)
    std::unordered_map<std::string, Vector3> vectorParams;  ///< Vector parameters (position, direction, etc.)
    std::unordered_map<std::string, std::string> stringParams; ///< String parameters (key, text, etc.)
    std::unordered_map<std::string, bool> boolParams;       ///< Boolean parameters
};

/**
 * @brief Block connection point
 */
struct ConnectionPoint {
    Vector2 position;           ///< Absolute position of connection point
    bool isOutput;              ///< True for output (bottom), false for input (top)
    int blockId;                ///< ID of the block this point belongs to
    
    ConnectionPoint(Vector2 pos, bool output, int id) 
        : position(pos), isOutput(output), blockId(id) {}
};

/**
 * @brief Block connection between two blocks
 */
struct BlockConnection {
    int fromBlockId;            ///< Source block ID
    int toBlockId;              ///< Target block ID
    Vector2 fromPoint;          ///< Connection point on source block
    Vector2 toPoint;            ///< Connection point on target block
    
    BlockConnection(int from, int to, Vector2 fromPos, Vector2 toPos)
        : fromBlockId(from), toBlockId(to), fromPoint(fromPos), toPoint(toPos) {}
};

/**
 * @brief Visual scripting block data
 */
struct ScriptBlock {
    int id;                    ///< Unique block ID
    BlockType type;            ///< Type of block
    Vector2 position;          ///< Free-form position on canvas
    Vector2 size;              ///< Block dimensions
    Color color;               ///< Block color
    std::string label;         ///< Block display text
    bool isDragging;           ///< Currently being dragged
    Vector2 dragOffset;        ///< Offset from mouse to block origin
    BlockConfig config;        ///< Block-specific configuration
    bool isOnCanvas;           ///< Whether block is on canvas (vs palette)
    int canvasOrder;          ///< Order of block on canvas for layering
    
    // Connection system
    bool hasInputConnection;    ///< Whether this block has an input connection
    bool hasOutputConnection;   ///< Whether this block has an output connection
    int connectedFromId;        ///< ID of block connected to input (-1 if none)
    int connectedToId;          ///< ID of block connected to output (-1 if none)
    bool isHighlightedForConnection; ///< Visual highlight for connection preview
    
    ScriptBlock(int blockId, BlockType blockType, Vector2 pos = {0, 0}) 
        : id(blockId), type(blockType), position(pos), size({120, 40}), 
          isDragging(false), dragOffset({0, 0}), isOnCanvas(false), canvasOrder(0),
          hasInputConnection(false), hasOutputConnection(false),
          connectedFromId(-1), connectedToId(-1), isHighlightedForConnection(false) {
        // Set default colors and labels based on type
        setBlockProperties();
        setDefaultConfig();
    }
    
    void setBlockProperties();
    void setDefaultConfig();
    std::string getDisplayLabel() const; ///< Get label with configuration parameters
    
    // Connection point helpers
    Vector2 getInputConnectionPoint() const;   ///< Get input connection point (top center)
    Vector2 getOutputConnectionPoint() const;  ///< Get output connection point (bottom center)
    Rectangle getConnectionBounds() const;     ///< Get bounds for connection detection
};

/**
 * @brief Visual script for an object (collection of blocks)
 */
struct VisualScript {
    int objectId;                           ///< Associated scene object ID
    std::vector<ScriptBlock> blocks;        ///< Blocks in this script
    std::vector<BlockConnection> connections; ///< Connections between blocks
    std::string name;                       ///< Script name
    bool enabled;                           ///< Whether script is active
    
    // Default constructor for std::unordered_map
    VisualScript() : objectId(-1), name("New Script"), enabled(true) {}
    
    VisualScript(int objId, const std::string& scriptName = "New Script")
        : objectId(objId), name(scriptName), enabled(true) {}
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
    
    // Visual scripting data
    std::unordered_map<int, VisualScript> _objectScripts; ///< Visual scripts per object ID
    int _nextBlockId = 1;                               ///< Next available block ID
    int _nextCanvasOrder = 0;                           ///< Next available canvas order
    
    // UI layout constants
    const float _leftPanelWidth = 250.0f;              ///< Width of left block palette panel
    const float _panelPadding = 10.0f;                 ///< Padding for panels
    
    // Block palette data
    std::vector<std::pair<BlockCategory, std::vector<BlockType>>> _blockPalette; ///< Organized block palette
    
    // Canvas state
    Vector2 _canvasOffset = {0, 0};                     ///< Canvas pan offset
    float _canvasZoom = 1.0f;                           ///< Canvas zoom level
    Rectangle _canvasBounds = {0, 0, 0, 0};            ///< Current canvas bounds
    
    // Drag and drop state
    bool _isMouseDragging = false;                      ///< Mouse drag state
    bool _isDraggingFromPalette = false;                ///< Dragging from palette vs canvas
    bool _isDraggingCanvasBlock = false;                ///< Dragging existing canvas block
    BlockType _draggedBlockType = BlockType::ON_START;  ///< Type of block being dragged from palette
    ScriptBlock* _draggedCanvasBlock = nullptr;         ///< Canvas block being dragged
    Vector2 _dragStartPos = {0, 0};                     ///< Initial drag position
    Vector2 _dragOffset = {0, 0};                       ///< Offset from mouse to block origin
    
    // Connection system
    const float _connectionSnapDistance = 30.0f;       ///< Distance for connection snapping
    ScriptBlock* _connectionCandidate = nullptr;        ///< Block candidate for connection
    bool _showConnectionPreview = false;                ///< Whether to show connection preview
    Vector2 _connectionPreviewStart = {0, 0};          ///< Start point for connection preview
    Vector2 _connectionPreviewEnd = {0, 0};            ///< End point for connection preview
    
    // Mouse state
    Vector2 _lastMousePos = {0, 0};                     ///< Last mouse position
    Vector2 _currentMousePos = {0, 0};                  ///< Current mouse position
    bool _leftMousePressed = false;                     ///< Left mouse button state
    bool _leftMouseReleased = false;                    ///< Left mouse release state
    bool _rightClickHandled = false;                    ///< Right click handled this frame
    
    // Configuration dialog state
    bool _showConfigDialog = false;                     ///< Whether config dialog is open
    ScriptBlock* _configuredBlock = nullptr;            ///< Block being configured
    
    // Canvas layout
    const float _blockSpacing = 10.0f;                 ///< Spacing between blocks on canvas
    const float _canvasMargin = 20.0f;                 ///< Margin from canvas edges
    
    // UI helper methods
    void drawBlockPalette(Rectangle bounds);            ///< Draw left block palette panel
    void drawScriptCanvas(Rectangle bounds);            ///< Draw central script canvas
    void drawCanvasGrid(Rectangle bounds);              ///< Draw grid background on canvas
    void drawCanvasOverlay(Rectangle bounds);           ///< Draw overlay when no object selected
    void drawBlock(const ScriptBlock& block, Vector2 offset = {0, 0}, bool shadow = true); ///< Draw a single block
    void drawConnections(const VisualScript& script);   ///< Draw connections between blocks
    void drawConnectionLine(Vector2 start, Vector2 end, Color color, float thickness = 3.0f); ///< Draw connection line
    void drawConnectionPoints(const ScriptBlock& block); ///< Draw connection points on block
    void drawPaletteCategory(const std::string& title, const std::vector<BlockType>& blocks, 
                           Rectangle bounds, float& yOffset); ///< Draw palette category
    void drawConfigDialog();                            ///< Draw block configuration dialog
    
    // Block management
    void initializeBlockPalette();                      ///< Initialize the block palette
    ScriptBlock createBlockFromType(BlockType type, Vector2 position); ///< Create block from type
    void addBlockToCanvas(BlockType type, Vector2 position); ///< Add block to current script
    void removeBlockFromCanvas(int blockId);            ///< Remove block from current script
    VisualScript& getCurrentScript();                   ///< Get current object's script
    std::string getBlockLabel(BlockType type) const;    ///< Get display label for block type
    Color getBlockColor(BlockType type) const;          ///< Get color for block type
    Vector2 getNextCanvasPosition();                    ///< Get next position for new canvas block
    void reorderCanvasBlocks();                         ///< Reorder and snap canvas blocks to grid
    
    // Connection management
    void updateBlockConnections();                      ///< Update connection candidates and highlights
    void createConnection(int fromBlockId, int toBlockId); ///< Create connection between blocks
    void removeConnection(int fromBlockId, int toBlockId); ///< Remove connection between blocks
    void removeAllConnectionsForBlock(int blockId);     ///< Remove all connections for a block
    ScriptBlock* findConnectionCandidate(const ScriptBlock& draggedBlock); ///< Find block to connect to
    void updateConnectedBlockPositions(int movedBlockId, Vector2 deltaPos); ///< Move connected blocks
    bool canBlocksConnect(const ScriptBlock& from, const ScriptBlock& to); ///< Check if blocks can connect
    float getDistanceBetweenConnectionPoints(const ScriptBlock& from, const ScriptBlock& to); ///< Distance between connection points
    
    // Mouse and drag handling
    void handleMouseInput(Vector2 mousePos, bool mousePressed, bool mouseReleased, bool rightClick);
    void startDragOperation(Vector2 mousePos);          ///< Start drag operation
    void updateDragOperation(Vector2 mousePos);         ///< Update ongoing drag operation
    void endDragOperation(Vector2 mousePos);            ///< End drag operation
    void handleRightClick(Vector2 mousePos);            ///< Handle right-click context menu
    void resetDragState();                              ///< Reset all drag state variables
    
    // Position and collision detection
    ScriptBlock* getCanvasBlockAtPosition(Vector2 pos); ///< Get canvas block at position
    BlockType getPaletteBlockTypeAtPosition(Vector2 pos); ///< Get palette block type at position
    bool isPointInBlock(Vector2 point, const ScriptBlock& block, Vector2 offset = {0, 0});
    bool isPositionInCanvas(Vector2 pos);               ///< Check if position is in canvas
    bool isPositionInPalette(Vector2 pos);              ///< Check if position is in palette
    Rectangle getPaletteBounds();                       ///< Get palette area bounds
    void updateCanvasBounds(Rectangle mainViewArea);    ///< Update canvas bounds
    
    // Configuration
    void openConfigDialog(ScriptBlock* block);          ///< Open configuration dialog for block
    void closeConfigDialog();                           ///< Close configuration dialog
    void applyBlockConfiguration();                     ///< Apply configuration changes
    
    // UI interaction
    void handleObjectSelection(int objectId);
};