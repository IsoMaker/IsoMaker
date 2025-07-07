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
    INVALID,      ///< Invalid/not found marker
    
    ON_START,
    ON_CLICK,
    ON_UPDATE,
    ON_KEY_PRESS,
    
    MOVE,
    ROTATE,
    CHANGE_COLOR,
    HIDE,
    SHOW,
    
    IF,
    LOOP,
    
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
    std::string label;         ///< Legacy label field (for compatibility)
    Color color;               ///< Legacy color field (for compatibility)
    bool isDragging;           ///< Currently being dragged
    Vector2 dragOffset;        ///< Offset from mouse to block origin
    BlockConfig config;        ///< Block-specific configuration
    bool isOnCanvas;           ///< Whether block is on canvas (vs palette)
    int canvasOrder;          ///< Order of block on canvas for layering
    bool isSelected;          ///< Whether block is currently selected
    bool isHovered;           ///< Whether mouse is hovering over block
    std::vector<ConnectionPoint> inputPorts;   ///< Input connection ports
    std::vector<ConnectionPoint> outputPorts;  ///< Output connection ports
    bool hasExecutionFlow;     ///< Whether block participates in execution flow
    bool canHaveBranches;      ///< Whether block can have multiple outputs (conditions)
    
    ScriptBlock(int blockId, BlockType blockType, Vector2 pos = {0, 0}) 
        : id(blockId), type(blockType), position(pos), size({160, 60}), 
          isDragging(false), dragOffset({0, 0}), isOnCanvas(false), canvasOrder(0),
          isSelected(false), isHovered(false),
          hasExecutionFlow(true), canHaveBranches(false) {
        setProfessionalBlockProperties();
        setDefaultConfig();
        setupConnectionPorts();
    }
    
    void setBlockProperties();
    void setProfessionalBlockProperties();
    void setDefaultConfig();
    void setupConnectionPorts();
    std::string getDisplayLabel() const;
    std::string getParameterSummary() const;
    
    Vector2 getInputConnectionPoint() const;
    Vector2 getOutputConnectionPoint() const;
    Rectangle getConnectionBounds() const;
    Rectangle getHeaderBounds() const;
    Rectangle getBodyBounds() const;
    bool isPointInHeader(Vector2 point) const;
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
    
    VisualScript() : objectId(-1), name("New Script"), enabled(true) {}
    
    VisualScript(int objId, const std::string& scriptName = "New Script")
        : objectId(objId), name(scriptName), enabled(true) {}
    
    CompiledScript compileToExecutionFlow() const;
    bool validateConnections(std::string& errors) const;
};

/**
 * @brief Visual scripting editor for the IsoMaker game engine
 */
class ScriptingEditor : public UI::ISceneProvider {
public:
    ScriptingEditor();
    ~ScriptingEditor();

    /**
     * @param window Reference to the application window
     * @param camera Reference to the rendering camera
     */
    void init(std::shared_ptr<Render::Window> window, std::shared_ptr<Render::Camera> camera);

    /**
     * @param inputHandler Reference to the input handler
     */
    void update(input::IHandlerBase &inputHandler);

    /**
     * @param mainViewArea Rectangle representing view area of scripting editor
     */
    void draw(Rectangle mainViewArea);

    void setupEventHandlers();

    /**
     * @param toolIndex Index of the newly selected tool
     */
    void handleToolChanged(int toolIndex);

    /**
     * @param actionType Type of file action to perform
     * @param filepath Optional file path for the operation
     */
    void handleFileAction(UI::EditorEventType actionType, const std::string& filepath = "");

    int getScriptCount() const;
    int getSelectedObjectId() const override;
    std::string getSelectedScriptName() const;
    
    /**
     * @param provider Pointer to the scene provider
     */
    void setSceneProvider(UI::ISceneProvider* provider);

    std::vector<UI::SceneObjectInfo> getSceneObjects() const override;
    UI::SceneObjectInfo getObjectInfo(int objectId) const override;
    bool selectObject(int objectId) override;
    bool deleteObject(int objectId) override;

private:
    std::shared_ptr<Render::Window> _window;
    std::shared_ptr<Render::Camera> _camera;

    int _currentTool = 0;
    int _selectedScriptId = -1;
    int _selectedObjectId = -1;
    
    bool _initialized = false;
    UI::ISceneProvider* _currentSceneProvider = nullptr;
    
    std::unordered_map<int, VisualScript> _objectScripts;
    int _nextBlockId = 1;
    int _nextCanvasOrder = 0;
    
    const float _leftPanelWidth = 250.0f;
    const float _rightPanelWidth = 250.0f;
    const float _panelPadding = 10.0f;
    
    std::vector<std::pair<BlockCategory, std::vector<BlockType>>> _blockPalette;
    
    Vector2 _canvasOffset = {0, 0};
    float _canvasZoom = 1.0f;
    Rectangle _canvasBounds = {0, 0, 0, 0};
    
    bool _isMouseDragging = false;
    bool _isDraggingFromPalette = false;
    bool _isDraggingCanvasBlock = false;
    BlockType _draggedBlockType = BlockType::ON_START;
    ScriptBlock* _draggedCanvasBlock = nullptr;
    Vector2 _dragStartPos = {0, 0};
    Vector2 _dragOffset = {0, 0};
    
    Vector2 _lastMousePos = {0, 0};
    Vector2 _currentMousePos = {0, 0};
    bool _leftMousePressed = false;
    bool _leftMouseReleased = false;
    bool _rightClickHandled = false;
    
    bool _showConfigDialog = false;
    ScriptBlock* _configuredBlock = nullptr;
    
    ScriptBlock* _selectedBlock = nullptr;
    ScriptBlock* _hoveredBlock = nullptr;
    
    struct ContextMenuItem {
        std::string id;
        std::string label;
        Color textColor;
        Color hoverColor;
        bool enabled;
        
        ContextMenuItem(const std::string& itemId, const std::string& itemLabel, 
                       Color text = WHITE, Color hover = GRAY, bool isEnabled = true)
            : id(itemId), label(itemLabel), textColor(text), hoverColor(hover), enabled(isEnabled) {}
    };
    
    struct ContextMenuState {
        bool isVisible = false;
        Vector2 position = {0, 0};
        ScriptBlock* targetBlock = nullptr;
        std::vector<ContextMenuItem> items;
        float width = 140.0f;
        float itemHeight = 32.0f;
        int hoveredItemIndex = -1;
        
        void clear() {
            isVisible = false;
            targetBlock = nullptr;
            items.clear();
            hoveredItemIndex = -1;
        }
        
        float getMenuHeight() const {
            return items.size() * itemHeight + 16.0f;
        }
    } _contextMenu;
    
    struct ConnectionDragState {
        bool isDragging = false;
        ScriptBlock* sourceBlock = nullptr;
        int sourcePortIndex = -1;
        ConnectionPortType sourcePortType;
        Vector2 dragStartPosition = {0, 0};
        Vector2 currentMousePosition = {0, 0};
        Color previewColor = WHITE;
        
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
    
    const float _blockSpacing = 10.0f;
    const float _canvasMargin = 20.0f;
    
    void drawBlockPalette(Rectangle bounds);
    void drawScriptCanvas(Rectangle bounds);
    void drawSceneObjectPanel(Rectangle bounds);
    void drawCanvasGrid(Rectangle bounds);
    void drawCanvasOverlay(Rectangle bounds);
    void drawProfessionalBlock(const ScriptBlock& block, Vector2 offset = {0, 0});
    void drawBlockHeader(const ScriptBlock& block, Rectangle headerRect, Vector2 offset = {0, 0});
    void drawBlockBody(const ScriptBlock& block, Rectangle bodyRect, Vector2 offset = {0, 0});
    void drawBlockPorts(const ScriptBlock& block, Vector2 offset = {0, 0});
    void drawPaletteCategory(const std::string& title, const std::vector<BlockType>& blocks, 
                           Rectangle bounds, float& yOffset);
    void drawConfigDialog();
    
    void initializeBlockPalette();
    ScriptBlock createBlockFromType(BlockType type, Vector2 position);
    void addBlockToCanvas(BlockType type, Vector2 position);
    void removeBlockFromCanvas(int blockId);
    VisualScript& getCurrentScript();
    std::string getBlockLabel(BlockType type) const;
    Color getBlockColor(BlockType type) const;
    Vector2 getNextCanvasPosition();
    void reorderCanvasBlocks();
    
    void handleMouseInput(Vector2 mousePos, bool mousePressed, bool mouseReleased, bool rightClick);
    void startDragOperation(Vector2 mousePos);
    void updateDragOperation(Vector2 mousePos);
    void endDragOperation(Vector2 mousePos);
    void resetDragState();
    
    ScriptBlock* getCanvasBlockAtPosition(Vector2 pos);
    BlockType getPaletteBlockTypeAtPosition(Vector2 pos);
    bool isPointInBlock(Vector2 point, const ScriptBlock& block, Vector2 offset = {0, 0});
    bool isPositionInCanvas(Vector2 pos);
    bool isPositionInPalette(Vector2 pos);
    Rectangle getPaletteBounds();
    void updateCanvasBounds(Rectangle mainViewArea);
    
    void openConfigDialog(ScriptBlock* block);
    void closeConfigDialog();
    void applyBlockConfiguration();
    
    void duplicateBlock(ScriptBlock* block);
    void deleteBlock(ScriptBlock* block);
    void editBlock(ScriptBlock* block);
    
    void showContextMenu(ScriptBlock* block, Vector2 mousePosition);
    void hideContextMenu();
    void drawContextMenu();
    void updateContextMenu(Vector2 mousePosition);
    void handleContextMenuAction(const std::string& actionId);
    Vector2 calculateContextMenuPosition(Vector2 requestedPos, float menuWidth, float menuHeight);
    void createContextMenuItems(ScriptBlock* block);
    void handleRightClick(Vector2 mousePosition);
    
    void updateConnectionSystem(Vector2 mousePosition, bool mousePressed, bool mouseReleased);
    void startConnectionDrag(ScriptBlock* block, int portIndex, ConnectionPortType portType, Vector2 startPos);
    void updateConnectionDrag(Vector2 mousePosition);
    void endConnectionDrag(Vector2 mousePosition);
    bool createConnection(ScriptBlock* fromBlock, int fromPortIndex, ScriptBlock* toBlock, int toPortIndex);
    void removeConnection(int fromBlockId, int toBlockId, int fromPortIndex = -1, int toPortIndex = -1);
    void removeAllConnectionsForBlock(int blockId);
    void drawConnections(const VisualScript& script);
    void drawConnectionLine(Vector2 start, Vector2 end, Color color, float thickness = 3.0f);
    void drawConnectionPreview();
    bool isConnectionValid(ScriptBlock* fromBlock, int fromPortIndex, ScriptBlock* toBlock, int toPortIndex);
    ConnectionPoint* getConnectionPointAt(Vector2 position, ScriptBlock** outBlock, int* outPortIndex);
    Color getConnectionColor(ConnectionPortType fromType, ConnectionPortType toType);
    void updateConnectionPositions(VisualScript& script);
    
    CompiledScript compileScript(int objectId);
    bool validateScript(int objectId, std::string& errors);
    void exportCompiledScripts(const std::string& filePath);
    
    void selectBlock(ScriptBlock* block);
    void deselectAllBlocks();
    void updateBlockHover(Vector2 mousePos);
    ScriptBlock* getBlockAtPosition(Vector2 pos);
    
    void handleObjectSelection(int objectId);
    
    void saveCurrentScript();
    void saveScriptToFile(const VisualScript& script, const std::string& filepath);
    void loadScriptFromFile(const std::string& filepath);
    void clearAllScripts();
    std::string scriptToJson(const VisualScript& script);
    VisualScript jsonToScript(const std::string& jsonContent);
    bool createDirectoryIfNotExists(const std::string& path);
    std::string getScriptsDirectory();
};