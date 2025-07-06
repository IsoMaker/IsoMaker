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
    LOOP,
    
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
 * @brief Connection port types for different execution flows
 */
enum class ConnectionPortType {
    EXECUTION_IN,    ///< Input execution flow (white triangle)
    EXECUTION_OUT,   ///< Output execution flow (white triangle)
    TRUE_OUT,        ///< True branch output (green)
    FALSE_OUT,       ///< False branch output (red)
    VALUE_IN,        ///< Value input (colored by type)
    VALUE_OUT        ///< Value output (colored by type)
};

/**
 * @brief Block connection point with enhanced typing
 */
struct ConnectionPoint {
    Vector2 position;           ///< Absolute position of connection point
    ConnectionPortType type;    ///< Type of connection port
    int blockId;                ///< ID of the block this point belongs to
    std::string label;          ///< Optional label for the port
    Color portColor;            ///< Color of the connection port
    
    ConnectionPoint(Vector2 pos, ConnectionPortType portType, int id, const std::string& portLabel = "", Color color = WHITE) 
        : position(pos), type(portType), blockId(id), label(portLabel), portColor(color) {}
};

/**
 * @brief Enhanced block connection with port type information
 */
struct BlockConnection {
    int fromBlockId;                    ///< Source block ID
    int toBlockId;                      ///< Target block ID
    ConnectionPortType fromPortType;    ///< Type of source port
    ConnectionPortType toPortType;      ///< Type of target port
    int fromPortIndex;                  ///< Index of source port on block
    int toPortIndex;                    ///< Index of target port on block
    Vector2 fromPoint;                  ///< Connection point on source block (cached)
    Vector2 toPoint;                    ///< Connection point on target block (cached)
    Color connectionColor;              ///< Visual color of the connection
    bool isValid;                       ///< Whether connection is valid
    
    BlockConnection(int from, int to, ConnectionPortType fromType, ConnectionPortType toType,
                   int fromIdx, int toIdx, Vector2 fromPos, Vector2 toPos, Color color = WHITE)
        : fromBlockId(from), toBlockId(to), fromPortType(fromType), toPortType(toType),
          fromPortIndex(fromIdx), toPortIndex(toIdx), fromPoint(fromPos), toPoint(toPos),
          connectionColor(color), isValid(true) {}
    
    // Legacy constructor for compatibility
    BlockConnection(int from, int to, Vector2 fromPos, Vector2 toPos)
        : fromBlockId(from), toBlockId(to), fromPortType(ConnectionPortType::EXECUTION_OUT),
          toPortType(ConnectionPortType::EXECUTION_IN), fromPortIndex(0), toPortIndex(0),
          fromPoint(fromPos), toPoint(toPos), connectionColor(WHITE), isValid(true) {}
};

/**
 * @brief Visual scripting block data with professional styling
 */
struct ScriptBlock {
    int id;                    ///< Unique block ID
    BlockType type;            ///< Type of block
    Vector2 position;          ///< Free-form position on canvas
    Vector2 size;              ///< Block dimensions
    Color primaryColor;        ///< Primary block color (category-based)
    Color secondaryColor;      ///< Secondary color for gradients/highlights
    Color headerColor;         ///< Header section color
    std::string title;         ///< Block title (main label)
    std::string subtitle;      ///< Subtitle or parameter summary
    
    // Legacy compatibility fields
    std::string label;         ///< Legacy label field (for compatibility)
    Color color;               ///< Legacy color field (for compatibility)
    bool isDragging;           ///< Currently being dragged
    Vector2 dragOffset;        ///< Offset from mouse to block origin
    BlockConfig config;        ///< Block-specific configuration
    bool isOnCanvas;           ///< Whether block is on canvas (vs palette)
    int canvasOrder;          ///< Order of block on canvas for layering
    bool isSelected;          ///< Whether block is currently selected
    bool isHovered;           ///< Whether mouse is hovering over block
    
    // Professional styling properties (managed by UI theme system)
    
    // Connection ports
    std::vector<ConnectionPoint> inputPorts;   ///< Input connection ports
    std::vector<ConnectionPoint> outputPorts;  ///< Output connection ports
    bool hasExecutionFlow;     ///< Whether block participates in execution flow
    bool canHaveBranches;      ///< Whether block can have multiple outputs (conditions)
    
    ScriptBlock(int blockId, BlockType blockType, Vector2 pos = {0, 0}) 
        : id(blockId), type(blockType), position(pos), size({160, 60}), 
          isDragging(false), dragOffset({0, 0}), isOnCanvas(false), canvasOrder(0),
          isSelected(false), isHovered(false),
          hasExecutionFlow(true), canHaveBranches(false) {
        // Set default colors and labels based on type
        setProfessionalBlockProperties();
        setDefaultConfig();
        setupConnectionPorts();
    }
    
    void setBlockProperties();                  ///< Legacy method (kept for compatibility)
    void setProfessionalBlockProperties();      ///< Set modern professional styling
    void setDefaultConfig();
    void setupConnectionPorts();               ///< Setup input/output ports based on block type
    std::string getDisplayLabel() const;        ///< Get label with configuration parameters
    std::string getParameterSummary() const;   ///< Get formatted parameter summary for subtitle
    
    // Connection point helpers
    Vector2 getInputConnectionPoint() const;   ///< Get input connection point (top center)
    Vector2 getOutputConnectionPoint() const;  ///< Get output connection point (bottom center)
    Rectangle getConnectionBounds() const;     ///< Get bounds for connection detection
    Rectangle getHeaderBounds() const;         ///< Get header section bounds
    Rectangle getBodyBounds() const;           ///< Get body section bounds
    bool isPointInHeader(Vector2 point) const; ///< Check if point is in header area
};

/**
 * @brief Visual script for an object (collection of blocks)
 */
/**
 * @brief Compiled script node for runtime execution
 */
struct CompiledScriptNode {
    int blockId;                            ///< Original block ID
    BlockType blockType;                    ///< Type of block
    BlockConfig config;                     ///< Block configuration parameters
    std::vector<int> nextNodes;             ///< IDs of next nodes to execute
    int trueNextNode = -1;                  ///< Next node for true branch (conditions)
    int falseNextNode = -1;                 ///< Next node for false branch (conditions)
    bool isEntryPoint = false;              ///< Whether this is a script entry point
    
    CompiledScriptNode(int id, BlockType type, const BlockConfig& cfg = {})
        : blockId(id), blockType(type), config(cfg) {}
};

/**
 * @brief Compiled script flow for runtime execution
 */
struct CompiledScript {
    int objectId;                           ///< Associated scene object ID
    std::string name;                       ///< Script name
    std::vector<CompiledScriptNode> nodes;  ///< Execution nodes in topological order
    std::vector<int> entryPoints;           ///< Entry point node IDs (OnStart, OnClick, etc.)
    bool isValid = false;                   ///< Whether compilation was successful
    std::string errors;                     ///< Compilation error messages
    
    CompiledScript(int objId, const std::string& scriptName = "New Script")
        : objectId(objId), name(scriptName) {}
};

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
    
    // Compilation methods
    CompiledScript compileToExecutionFlow() const;     ///< Compile visual script to execution flow
    bool validateConnections(std::string& errors) const; ///< Validate all connections
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
    const float _rightPanelWidth = 250.0f;             ///< Width of right scene objects panel
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
    
    
    // Mouse state
    Vector2 _lastMousePos = {0, 0};                     ///< Last mouse position
    Vector2 _currentMousePos = {0, 0};                  ///< Current mouse position
    bool _leftMousePressed = false;                     ///< Left mouse button state
    bool _leftMouseReleased = false;                    ///< Left mouse release state
    bool _rightClickHandled = false;                    ///< Right click handled this frame
    
    // Configuration dialog state
    bool _showConfigDialog = false;                     ///< Whether config dialog is open
    ScriptBlock* _configuredBlock = nullptr;            ///< Block being configured
    
    
    // Block selection and interaction
    ScriptBlock* _selectedBlock = nullptr;              ///< Currently selected block
    ScriptBlock* _hoveredBlock = nullptr;               ///< Currently hovered block
    
    // Modern Context Menu System
    /**
     * @brief Context menu item definition
     */
    struct ContextMenuItem {
        std::string id;              ///< Unique identifier for the action
        std::string label;           ///< Display text
        Color textColor;             ///< Text color
        Color hoverColor;            ///< Background color when hovered
        bool enabled;                ///< Whether the item is clickable
        
        ContextMenuItem(const std::string& itemId, const std::string& itemLabel, 
                       Color text = WHITE, Color hover = GRAY, bool isEnabled = true)
            : id(itemId), label(itemLabel), textColor(text), hoverColor(hover), enabled(isEnabled) {}
    };
    
    /**
     * @brief Context menu state and management
     */
    struct ContextMenuState {
        bool isVisible = false;                          ///< Whether menu is currently shown
        Vector2 position = {0, 0};                       ///< Menu position on screen
        ScriptBlock* targetBlock = nullptr;              ///< Block the menu is associated with
        std::vector<ContextMenuItem> items;              ///< Menu items to display
        float width = 140.0f;                            ///< Menu width
        float itemHeight = 32.0f;                        ///< Height of each menu item
        int hoveredItemIndex = -1;                       ///< Index of currently hovered item
        
        void clear() {
            isVisible = false;
            targetBlock = nullptr;
            items.clear();
            hoveredItemIndex = -1;
        }
        
        float getMenuHeight() const {
            return items.size() * itemHeight + 16.0f; // 2 * 8px padding
        }
    } _contextMenu;
    
    // Modern Connection System
    /**
     * @brief Connection drag state for creating new connections
     */
    struct ConnectionDragState {
        bool isDragging = false;                         ///< Whether actively dragging to create connection
        ScriptBlock* sourceBlock = nullptr;             ///< Block where drag started
        int sourcePortIndex = -1;                       ///< Index of source port
        ConnectionPortType sourcePortType;              ///< Type of source port
        Vector2 dragStartPosition = {0, 0};             ///< World position where drag started
        Vector2 currentMousePosition = {0, 0};          ///< Current mouse position for preview
        Color previewColor = WHITE;                     ///< Color for connection preview
        
        void clear() {
            isDragging = false;
            sourceBlock = nullptr;
            sourcePortIndex = -1;
            dragStartPosition = {0, 0};
            currentMousePosition = {0, 0};
        }
        
        bool isValid() const {
            return isDragging && sourceBlock != nullptr && sourcePortIndex >= 0;
        }
    } _connectionDrag;
    
    // Canvas layout
    const float _blockSpacing = 10.0f;                 ///< Spacing between blocks on canvas
    const float _canvasMargin = 20.0f;                 ///< Margin from canvas edges
    
    // UI helper methods
    void drawBlockPalette(Rectangle bounds);            ///< Draw left block palette panel
    void drawScriptCanvas(Rectangle bounds);            ///< Draw central script canvas
    void drawSceneObjectPanel(Rectangle bounds);        ///< Draw right scene objects panel
    void drawCanvasGrid(Rectangle bounds);              ///< Draw grid background on canvas
    void drawCanvasOverlay(Rectangle bounds);           ///< Draw overlay when no object selected
    void drawProfessionalBlock(const ScriptBlock& block, Vector2 offset = {0, 0}); ///< Draw modern styled block
    void drawBlockHeader(const ScriptBlock& block, Rectangle headerRect, Vector2 offset = {0, 0}); ///< Draw block header
    void drawBlockBody(const ScriptBlock& block, Rectangle bodyRect, Vector2 offset = {0, 0}); ///< Draw block body with parameters
    void drawBlockPorts(const ScriptBlock& block, Vector2 offset = {0, 0}); ///< Draw connection ports
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
    
    
    // Mouse and drag handling
    void handleMouseInput(Vector2 mousePos, bool mousePressed, bool mouseReleased, bool rightClick);
    void startDragOperation(Vector2 mousePos);          ///< Start drag operation
    void updateDragOperation(Vector2 mousePos);         ///< Update ongoing drag operation
    void endDragOperation(Vector2 mousePos);            ///< End drag operation
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
    
    void duplicateBlock(ScriptBlock* block);            ///< Duplicate a block with same parameters
    void deleteBlock(ScriptBlock* block);               ///< Delete a block and its connections
    void editBlock(ScriptBlock* block);                 ///< Open edit dialog for block
    
    // Modern Context Menu System Methods
    void showContextMenu(ScriptBlock* block, Vector2 mousePosition); ///< Show context menu for a block
    void hideContextMenu();                             ///< Hide the context menu
    void drawContextMenu();                             ///< Render the context menu
    void updateContextMenu(Vector2 mousePosition);     ///< Update context menu state and handle interactions
    void handleContextMenuAction(const std::string& actionId); ///< Execute context menu action
    Vector2 calculateContextMenuPosition(Vector2 requestedPos, float menuWidth, float menuHeight); ///< Calculate optimal menu position
    void createContextMenuItems(ScriptBlock* block);   ///< Create appropriate menu items for the block
    void handleRightClick(Vector2 mousePosition);      ///< Handle right-click events
    
    // Modern Connection System Methods
    void updateConnectionSystem(Vector2 mousePosition, bool mousePressed, bool mouseReleased); ///< Update connection drag system
    void startConnectionDrag(ScriptBlock* block, int portIndex, ConnectionPortType portType, Vector2 startPos); ///< Start connection creation
    void updateConnectionDrag(Vector2 mousePosition);  ///< Update connection drag preview
    void endConnectionDrag(Vector2 mousePosition);     ///< Finish connection creation
    bool createConnection(ScriptBlock* fromBlock, int fromPortIndex, ScriptBlock* toBlock, int toPortIndex); ///< Create new connection
    void removeConnection(int fromBlockId, int toBlockId, int fromPortIndex = -1, int toPortIndex = -1); ///< Remove specific connection
    void removeAllConnectionsForBlock(int blockId);    ///< Remove all connections involving a block
    void drawConnections(const VisualScript& script);  ///< Draw all connections in script
    void drawConnectionLine(Vector2 start, Vector2 end, Color color, float thickness = 3.0f); ///< Draw connection line with curve
    void drawConnectionPreview();                      ///< Draw connection drag preview
    bool isConnectionValid(ScriptBlock* fromBlock, int fromPortIndex, ScriptBlock* toBlock, int toPortIndex); ///< Validate connection
    ConnectionPoint* getConnectionPointAt(Vector2 position, ScriptBlock** outBlock, int* outPortIndex); ///< Get connection point at position
    Color getConnectionColor(ConnectionPortType fromType, ConnectionPortType toType); ///< Get connection visual color
    void updateConnectionPositions(VisualScript& script); ///< Update cached connection positions
    
    // Script Compilation System
    CompiledScript compileScript(int objectId);           ///< Compile visual script to execution flow
    bool validateScript(int objectId, std::string& errors); ///< Validate script connections and structure
    void exportCompiledScripts(const std::string& filePath); ///< Export all compiled scripts to file
    
    // Block selection and interaction
    void selectBlock(ScriptBlock* block);               ///< Select a block
    void deselectAllBlocks();                           ///< Deselect all blocks
    void updateBlockHover(Vector2 mousePos);            ///< Update block hover states
    ScriptBlock* getBlockAtPosition(Vector2 pos);       ///< Get block at mouse position
    
    // UI interaction
    void handleObjectSelection(int objectId);
};