/**
 * @file ScriptingEditor.cpp
 * @brief Implementation of the ScriptingEditor class with visual block-based scripting
 * @author IsoMaker Team
 * @version 0.2
 */

#include "ScriptingEditor.hpp"
#include "../../UI/UITheme.hpp"
#include <iostream>
#include <algorithm>

ScriptingEditor::ScriptingEditor() {
    std::cout << "[ScriptingEditor] Constructor called" << std::endl;
}

ScriptingEditor::~ScriptingEditor() {
    std::cout << "[ScriptingEditor] Destructor called" << std::endl;
}

void ScriptingEditor::init(std::shared_ptr<Render::Window> window, std::shared_ptr<Render::Camera> camera) {
    _window = window;
    _camera = camera;
    
    setupEventHandlers();
    initializeBlockPalette();
    _initialized = true;
    
    std::cout << "[ScriptingEditor] Initialized successfully" << std::endl;
}

void ScriptingEditor::update(input::IHandlerBase &inputHandler) {
    if (!_initialized) {
        return;
    }
    
    // Get mouse state
    Vector2D mousePosD = inputHandler.getCursorCoords();
    Vector2 mousePos = {(float)mousePosD.x, (float)mousePosD.y};
    bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    bool mouseReleased = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    bool rightClick = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
    
    // Update mouse state
    _lastMousePos = _currentMousePos;
    _currentMousePos = mousePos;
    _leftMousePressed = mousePressed;
    _leftMouseReleased = mouseReleased;
    
    // Update block hover states
    updateBlockHover(mousePos);
    
    // Handle mouse input for drag and drop
    handleMouseInput(mousePos, mousePressed, mouseReleased, rightClick);
}

void ScriptingEditor::draw(Rectangle mainViewArea) {
    if (!_initialized) {
        return;
    }
    
    // Update canvas bounds for mouse collision detection
    updateCanvasBounds(mainViewArea);
    
    // Clear the main view area with background
    DrawRectangleRec(mainViewArea, UI::BACKGROUND);
    
    // Calculate panel layouts with right panel for scene objects
    Rectangle leftPanel = {
        mainViewArea.x,
        mainViewArea.y,
        _leftPanelWidth,
        mainViewArea.height
    };
    
    Rectangle rightPanel = {
        mainViewArea.x + mainViewArea.width - _rightPanelWidth,
        mainViewArea.y,
        _rightPanelWidth,
        mainViewArea.height
    };
    
    Rectangle canvasPanel = {
        mainViewArea.x + _leftPanelWidth + _panelPadding,
        mainViewArea.y,
        mainViewArea.width - _leftPanelWidth - _rightPanelWidth - (_panelPadding * 2),
        mainViewArea.height
    };
    
    // Draw panels
    drawBlockPalette(leftPanel);
    drawScriptCanvas(canvasPanel);
    drawSceneObjectPanel(rightPanel);
    
    // Draw drag preview if dragging from palette
    if (_isDraggingFromPalette) {
        ScriptBlock previewBlock(0, _draggedBlockType, _currentMousePos);
        previewBlock.primaryColor.a = 150; // Semi-transparent
        drawProfessionalBlock(previewBlock, {-previewBlock.size.x/2, -previewBlock.size.y/2});
    }
    
    // Draw configuration dialog on top if open
    if (_showConfigDialog) {
        drawConfigDialog();
    }
    
    // Draw context menu on top if open
    if (_showContextMenu) {
        drawContextMenu();
    }
    
    // Draw border around the main area
    DrawRectangleLinesEx(mainViewArea, 1, UI::UI_PRIMARY);
}

void ScriptingEditor::setupEventHandlers() {
    // Subscribe to relevant events from the UI
    UI::g_eventDispatcher.subscribe(UI::EditorEventType::TOOL_CHANGED, 
        [this](const UI::EditorEvent& event) {
            if (std::holds_alternative<int>(event.data)) {
                handleToolChanged(std::get<int>(event.data));
            }
        });
    
    UI::g_eventDispatcher.subscribe(UI::EditorEventType::FILE_NEW, 
        [this](const UI::EditorEvent& event) {
            handleFileAction(UI::EditorEventType::FILE_NEW);
        });
    
    UI::g_eventDispatcher.subscribe(UI::EditorEventType::FILE_SAVE, 
        [this](const UI::EditorEvent& event) {
            handleFileAction(UI::EditorEventType::FILE_SAVE);
        });
    
    UI::g_eventDispatcher.subscribe(UI::EditorEventType::FILE_OPEN, 
        [this](const UI::EditorEvent& event) {
            handleFileAction(UI::EditorEventType::FILE_OPEN);
        });
    
    std::cout << "[ScriptingEditor] Event handlers set up" << std::endl;
}

void ScriptingEditor::handleToolChanged(int toolIndex) {
    _currentTool = toolIndex;
    std::cout << "[ScriptingEditor] Tool changed to: " << toolIndex << std::endl;
}

void ScriptingEditor::handleFileAction(UI::EditorEventType actionType, const std::string& filepath) {
    switch (actionType) {
        case UI::EditorEventType::FILE_NEW:
            std::cout << "[ScriptingEditor] New script project" << std::endl;
            break;
        case UI::EditorEventType::FILE_SAVE:
            std::cout << "[ScriptingEditor] Save scripts" << std::endl;
            break;
        case UI::EditorEventType::FILE_OPEN:
            std::cout << "[ScriptingEditor] Open script project" << std::endl;
            break;
        default:
            break;
    }
}

int ScriptingEditor::getScriptCount() const {
    return _objectScripts.size();
}

int ScriptingEditor::getSelectedObjectId() const {
    // Forward to the current scene provider (usually the Map Editor)
    if (_currentSceneProvider) {
        return _currentSceneProvider->getSelectedObjectId();
    }
    return _selectedObjectId;
}

std::string ScriptingEditor::getSelectedScriptName() const {
    if (_selectedObjectId == -1) {
        return "";
    }
    auto it = _objectScripts.find(_selectedObjectId);
    if (it != _objectScripts.end()) {
        return it->second.name;
    }
    return "Script_" + std::to_string(_selectedObjectId);
}

void ScriptingEditor::setSceneProvider(UI::ISceneProvider* provider) {
    _currentSceneProvider = provider;
}

// ISceneProvider interface implementation
std::vector<UI::SceneObjectInfo> ScriptingEditor::getSceneObjects() const {
    // Forward to the current scene provider (usually the Map Editor)
    if (_currentSceneProvider) {
        return _currentSceneProvider->getSceneObjects();
    }
    // Return empty vector if no scene provider is set
    return std::vector<UI::SceneObjectInfo>();
}

UI::SceneObjectInfo ScriptingEditor::getObjectInfo(int objectId) const {
    // Forward to the current scene provider (usually the Map Editor)
    if (_currentSceneProvider) {
        return _currentSceneProvider->getObjectInfo(objectId);
    }
    // Return placeholder info if no scene provider is set
    UI::SceneObjectInfo info;
    info.id = objectId;
    info.name = "Script_" + std::to_string(objectId);
    info.type = UI::SceneObjectType::UNKNOWN;
    info.position = {0.0f, 0.0f, 0.0f};
    return info;
}

bool ScriptingEditor::selectObject(int objectId) {
    // Forward to the current scene provider (usually the Map Editor)
    bool result = false;
    if (_currentSceneProvider) {
        result = _currentSceneProvider->selectObject(objectId);
    }
    
    // Also update our local selection
    _selectedObjectId = objectId;
    std::cout << "[ScriptingEditor] Selected object ID: " << objectId << std::endl;
    return result;
}

bool ScriptingEditor::deleteObject(int objectId) {
    // Forward to the current scene provider (usually the Map Editor)
    if (_currentSceneProvider) {
        bool result = _currentSceneProvider->deleteObject(objectId);
        std::cout << "[ScriptingEditor] Forwarded delete request for object ID: " << objectId << std::endl;
        return result;
    }
    
    std::cout << "[ScriptingEditor] Cannot delete object ID: " << objectId << " - no scene provider set" << std::endl;
    return false;
}

// Block management implementation
void ScriptingEditor::initializeBlockPalette() {
    _blockPalette.clear();
    
    // Events category
    _blockPalette.push_back({BlockCategory::EVENTS, {
        BlockType::ON_START, BlockType::ON_CLICK, BlockType::ON_UPDATE, BlockType::ON_KEY_PRESS
    }});
    
    // Actions category
    _blockPalette.push_back({BlockCategory::ACTIONS, {
        BlockType::MOVE, BlockType::ROTATE, BlockType::CHANGE_COLOR, 
        BlockType::HIDE, BlockType::SHOW
    }});
    
    // Conditions category
    _blockPalette.push_back({BlockCategory::CONDITIONS, {
        BlockType::IF, BlockType::LOOP
    }});
    
    // Miscellaneous category
    _blockPalette.push_back({BlockCategory::MISC, {
        BlockType::TRUE, BlockType::FALSE, BlockType::VALUE, 
        BlockType::ENTITY, BlockType::DELAY, BlockType::LOG
    }});
}

void ScriptBlock::setBlockProperties() {
    switch (type) {
        // Special
        case BlockType::INVALID:
            label = "Invalid";
            color = {255, 0, 0, 255}; // Red
            break;
            
        // Events - Blue
        case BlockType::ON_START:
            label = "OnStart";
            color = {35, 100, 155, 255}; // Blue
            break;
        case BlockType::ON_CLICK:
            label = "OnClick";
            color = {35, 100, 155, 255}; // Blue
            break;
        case BlockType::ON_UPDATE:
            label = "OnUpdate";
            color = {35, 100, 155, 255}; // Blue
            break;
        case BlockType::ON_KEY_PRESS:
            label = "On key press Space";
            color = {35, 100, 155, 255}; // Blue
            size = {160, 40}; // Wider for longer text
            break;
            
        // Actions - Green
        case BlockType::MOVE:
            label = "Move";
            color = {28, 115, 48, 255}; // Green
            break;
        case BlockType::ROTATE:
            label = "Rotate";
            color = {28, 115, 48, 255}; // Green
            break;
        case BlockType::CHANGE_COLOR:
            label = "Change Color";
            color = {28, 115, 48, 255}; // Green
            size = {140, 40}; // Wider for longer text
            break;
        case BlockType::HIDE:
            label = "Hide";
            color = {28, 115, 48, 255}; // Green
            break;
        case BlockType::SHOW:
            label = "Show";
            color = {28, 115, 48, 255}; // Green
            break;
            
        // Conditions - Orange
        case BlockType::IF:
            label = "If";
            color = {230, 120, 20, 255}; // Orange
            break;
        case BlockType::LOOP:
            label = "Loop";
            color = {230, 120, 20, 255}; // Orange
            break;
            
        // Miscellaneous - Gray
        case BlockType::TRUE:
            label = "True";
            color = {75, 80, 85, 255}; // Gray
            break;
        case BlockType::FALSE:
            label = "False";
            color = {75, 80, 85, 255}; // Gray
            break;
        case BlockType::VALUE:
            label = "Value";
            color = {75, 80, 85, 255}; // Gray
            break;
        case BlockType::ENTITY:
            label = "Entity";
            color = {75, 80, 85, 255}; // Gray
            break;
        case BlockType::DELAY:
            label = "Delay";
            color = {75, 80, 85, 255}; // Gray
            break;
        case BlockType::LOG:
            label = "Log";
            color = {75, 80, 85, 255}; // Gray
            break;
    }
}

void ScriptBlock::setProfessionalBlockProperties() {
    // Set legacy properties for compatibility
    setBlockProperties();
    
    // Modern professional styling based on block category
    switch (type) {
        // Special blocks
        case BlockType::INVALID:
            title = "Invalid Block";
            subtitle = "Error";
            primaryColor = {220, 53, 69, 255};     // Bootstrap danger red
            secondaryColor = {248, 215, 218, 255}; // Light red
            headerColor = {176, 42, 55, 255};      // Darker red
            hasExecutionFlow = false;
            canHaveBranches = false;
            break;
            
        // Event blocks - Using harmonized theme colors
        case BlockType::ON_START:
            title = "On Start";
            subtitle = "When object starts";
            primaryColor = UI::BLOCK_EVENT;           // Harmonized event red
            secondaryColor = UI::BLOCK_EVENT_SECONDARY;
            headerColor = UI::BLOCK_EVENT;
            hasExecutionFlow = true;
            canHaveBranches = false;
            size = {180, 70};
            break;
            
        case BlockType::ON_CLICK:
            title = "On Click";
            subtitle = "When object is clicked";
            primaryColor = UI::BLOCK_EVENT;
            secondaryColor = UI::BLOCK_EVENT_SECONDARY;
            headerColor = UI::BLOCK_EVENT;
            hasExecutionFlow = true;
            canHaveBranches = false;
            size = {180, 70};
            break;
            
        case BlockType::ON_UPDATE:
            title = "On Update";
            subtitle = "Every frame";
            primaryColor = UI::BLOCK_EVENT;
            secondaryColor = UI::BLOCK_EVENT_SECONDARY;
            headerColor = UI::BLOCK_EVENT;
            hasExecutionFlow = true;
            canHaveBranches = false;
            size = {180, 70};
            break;
            
        case BlockType::ON_KEY_PRESS:
            title = "On Key Press";
            subtitle = "When key is pressed";
            primaryColor = UI::BLOCK_EVENT;
            secondaryColor = UI::BLOCK_EVENT_SECONDARY;
            headerColor = UI::BLOCK_EVENT;
            hasExecutionFlow = true;
            canHaveBranches = false;
            size = {200, 80};
            break;
            
        // Action blocks - Using harmonized theme colors
        case BlockType::MOVE:
            title = "Move";
            subtitle = "Change position";
            primaryColor = UI::BLOCK_ACTION;          // Harmonized action blue
            secondaryColor = UI::BLOCK_ACTION_SECONDARY;
            headerColor = UI::BLOCK_ACTION;
            hasExecutionFlow = true;
            canHaveBranches = false;
            size = {180, 80};
            break;
            
        case BlockType::ROTATE:
            title = "Rotate";
            subtitle = "Change rotation";
            primaryColor = UI::BLOCK_ACTION;
            secondaryColor = UI::BLOCK_ACTION_SECONDARY;
            headerColor = UI::BLOCK_ACTION;
            hasExecutionFlow = true;
            canHaveBranches = false;
            size = {180, 80};
            break;
            
        case BlockType::CHANGE_COLOR:
            title = "Change Color";
            subtitle = "Modify appearance";
            primaryColor = UI::BLOCK_ACTION;
            secondaryColor = UI::BLOCK_ACTION_SECONDARY;
            headerColor = UI::BLOCK_ACTION;
            hasExecutionFlow = true;
            canHaveBranches = false;
            size = {190, 80};
            break;
            
        case BlockType::HIDE:
            title = "Hide";
            subtitle = "Make invisible";
            primaryColor = UI::BLOCK_ACTION;
            secondaryColor = UI::BLOCK_ACTION_SECONDARY;
            headerColor = UI::BLOCK_ACTION;
            hasExecutionFlow = true;
            canHaveBranches = false;
            size = {160, 70};
            break;
            
        case BlockType::SHOW:
            title = "Show";
            subtitle = "Make visible";
            primaryColor = UI::BLOCK_ACTION;
            secondaryColor = UI::BLOCK_ACTION_SECONDARY;
            headerColor = UI::BLOCK_ACTION;
            hasExecutionFlow = true;
            canHaveBranches = false;
            size = {160, 70};
            break;
            
        // Condition blocks - Using harmonized theme colors
        case BlockType::IF:
            title = "If";
            subtitle = "Conditional execution";
            primaryColor = UI::BLOCK_CONDITION;       // Harmonized condition green
            secondaryColor = UI::BLOCK_CONDITION_SECONDARY;
            headerColor = UI::BLOCK_CONDITION;
            hasExecutionFlow = false; // No input execution port
            canHaveBranches = true; // Has true/false outputs
            size = {180, 90};
            break;
            
        case BlockType::LOOP:
            title = "Loop";
            subtitle = "Repeat N times";
            primaryColor = UI::BLOCK_CONDITION;
            secondaryColor = UI::BLOCK_CONDITION_SECONDARY;
            headerColor = UI::BLOCK_CONDITION;
            hasExecutionFlow = true;
            canHaveBranches = true; // Has body and next outputs
            size = {180, 100};
            break;
            
        // Value blocks - Using harmonized theme colors
        case BlockType::TRUE:
            title = "True";
            subtitle = "Boolean value";
            primaryColor = UI::BLOCK_VALUE;           // Harmonized value yellow
            secondaryColor = UI::BLOCK_VALUE_SECONDARY;
            headerColor = UI::BLOCK_VALUE;
            hasExecutionFlow = false;
            canHaveBranches = false;
            size = {140, 60};
            break;
            
        case BlockType::FALSE:
            title = "False";
            subtitle = "Boolean value";
            primaryColor = UI::BLOCK_VALUE;
            secondaryColor = UI::BLOCK_VALUE_SECONDARY;
            headerColor = UI::BLOCK_VALUE;
            hasExecutionFlow = false;
            canHaveBranches = false;
            size = {140, 60};
            break;
            
        case BlockType::VALUE:
            title = "Value";
            subtitle = "Number value";
            primaryColor = UI::BLOCK_VALUE;
            secondaryColor = UI::BLOCK_VALUE_SECONDARY;
            headerColor = UI::BLOCK_VALUE;
            hasExecutionFlow = false;
            canHaveBranches = false;
            size = {160, 70};
            break;
            
        case BlockType::ENTITY:
            title = "Entity";
            subtitle = "Object reference";
            primaryColor = UI::BLOCK_VALUE;
            secondaryColor = UI::BLOCK_VALUE_SECONDARY;
            headerColor = UI::BLOCK_VALUE;
            hasExecutionFlow = false;
            canHaveBranches = false;
            size = {160, 70};
            break;
            
        // Utility blocks - Using harmonized theme colors
        case BlockType::DELAY:
            title = "Delay";
            subtitle = "Wait for time";
            primaryColor = UI::BLOCK_MISC;            // Harmonized misc gray
            secondaryColor = UI::BLOCK_MISC_SECONDARY;
            headerColor = UI::BLOCK_MISC;
            hasExecutionFlow = true;
            canHaveBranches = false;
            size = {180, 80};
            break;
            
        case BlockType::LOG:
            title = "Log";
            subtitle = "Debug output";
            primaryColor = UI::BLOCK_MISC;
            secondaryColor = UI::BLOCK_MISC_SECONDARY;
            headerColor = UI::BLOCK_MISC;
            hasExecutionFlow = true;
            canHaveBranches = false;
            size = {180, 80};
            break;
    }
    
    // Enhanced styling is handled by the drawing functions using UI theme
    
    // Update legacy fields for compatibility
    label = title;
    color = primaryColor;
}

void ScriptBlock::setDefaultConfig() {
    switch (type) {
        case BlockType::ON_KEY_PRESS:
            config.stringParams["key"] = "Space";
            break;
        case BlockType::MOVE:
            config.vectorParams["direction"] = {1.0f, 0.0f, 0.0f};
            config.floatParams["speed"] = 1.0f;
            break;
        case BlockType::ROTATE:
            config.vectorParams["axis"] = {0.0f, 1.0f, 0.0f};
            config.floatParams["speed"] = 90.0f;
            break;
        case BlockType::CHANGE_COLOR:
            config.vectorParams["color"] = {1.0f, 1.0f, 1.0f};
            break;
        case BlockType::DELAY:
            config.floatParams["duration"] = 1.0f;
            break;
        case BlockType::VALUE:
            config.floatParams["value"] = 0.0f;
            break;
        case BlockType::LOG:
            config.stringParams["message"] = "Debug message";
            break;
        case BlockType::LOOP:
            config.floatParams["iterations"] = 5.0f;
            break;
        default:
            // No configuration needed for other blocks
            break;
    }
}

std::string ScriptBlock::getDisplayLabel() const {
    switch (type) {
        case BlockType::ON_KEY_PRESS: {
            auto keyIt = config.stringParams.find("key");
            std::string key = (keyIt != config.stringParams.end()) ? keyIt->second : "Space";
            return "On key press " + key;
        }
        case BlockType::MOVE: {
            auto speedIt = config.floatParams.find("speed");
            float speed = (speedIt != config.floatParams.end()) ? speedIt->second : 1.0f;
            return "Move (speed: " + std::to_string((int)speed) + ")";
        }
        case BlockType::ROTATE: {
            auto speedIt = config.floatParams.find("speed");
            float speed = (speedIt != config.floatParams.end()) ? speedIt->second : 90.0f;
            return "Rotate (" + std::to_string((int)speed) + "°/s)";
        }
        case BlockType::DELAY: {
            auto durationIt = config.floatParams.find("duration");
            float duration = (durationIt != config.floatParams.end()) ? durationIt->second : 1.0f;
            return "Delay " + std::to_string(duration) + "s";
        }
        case BlockType::VALUE: {
            auto valueIt = config.floatParams.find("value");
            float value = (valueIt != config.floatParams.end()) ? valueIt->second : 0.0f;
            return "Value: " + std::to_string((int)value);
        }
        case BlockType::LOG: {
            auto messageIt = config.stringParams.find("message");
            std::string message = (messageIt != config.stringParams.end()) ? messageIt->second : "Debug message";
            return "Log: " + message.substr(0, 10) + "...";
        }
        default:
            return label;
    }
}

// Connection point helper methods
Vector2 ScriptBlock::getInputConnectionPoint() const {
    return Vector2{position.x + size.x / 2, position.y};
}

Vector2 ScriptBlock::getOutputConnectionPoint() const {
    return Vector2{position.x + size.x / 2, position.y + size.y};
}

Rectangle ScriptBlock::getConnectionBounds() const {
    return Rectangle{position.x - 10, position.y - 10, size.x + 20, size.y + 20};
}

// New professional block helper methods
std::string ScriptBlock::getParameterSummary() const {
    switch (type) {
        case BlockType::ON_KEY_PRESS: {
            auto keyIt = config.stringParams.find("key");
            std::string key = (keyIt != config.stringParams.end()) ? keyIt->second : "Space";
            return "Key: " + key;
        }
        case BlockType::MOVE: {
            auto speedIt = config.floatParams.find("speed");
            float speed = (speedIt != config.floatParams.end()) ? speedIt->second : 1.0f;
            return "Speed: " + std::to_string((int)speed);
        }
        case BlockType::ROTATE: {
            auto speedIt = config.floatParams.find("speed");
            float speed = (speedIt != config.floatParams.end()) ? speedIt->second : 90.0f;
            return std::to_string((int)speed) + "°/s";
        }
        case BlockType::DELAY: {
            auto durationIt = config.floatParams.find("duration");
            float duration = (durationIt != config.floatParams.end()) ? durationIt->second : 1.0f;
            return std::to_string(duration) + "s";
        }
        case BlockType::VALUE: {
            auto valueIt = config.floatParams.find("value");
            float value = (valueIt != config.floatParams.end()) ? valueIt->second : 0.0f;
            return std::to_string((int)value);
        }
        case BlockType::LOOP: {
            auto iterIt = config.floatParams.find("iterations");
            float iterations = (iterIt != config.floatParams.end()) ? iterIt->second : 5.0f;
            return std::to_string((int)iterations) + " times";
        }
        default:
            return subtitle;
    }
}

void ScriptBlock::setupConnectionPorts() {
    inputPorts.clear();
    outputPorts.clear();
    
    if (hasExecutionFlow) {
        // Add execution input port (top center)
        Vector2 inputPos = {position.x + size.x / 2, position.y};
        inputPorts.push_back(ConnectionPoint(inputPos, ConnectionPortType::EXECUTION_IN, id, "In", WHITE));
        
        if (canHaveBranches) {
            if (type == BlockType::LOOP) {
                // Loop has Next (left) and Body (right) outputs
                Vector2 nextOut = {position.x, position.y + size.y / 2};   // Left = Exit/Next
                Vector2 bodyOut = {position.x + size.x, position.y + size.y / 2};   // Right = Loop Body
                outputPorts.push_back(ConnectionPoint(nextOut, ConnectionPortType::EXECUTION_OUT, id, "Next", {220, 53, 69, 255})); // Red for exit
                outputPorts.push_back(ConnectionPoint(bodyOut, ConnectionPortType::EXECUTION_OUT, id, "Body", {25, 135, 84, 255})); // Green for body
            } else {
                // Other conditional blocks have false (left) and true (right) outputs
                Vector2 falseOut = {position.x + size.x * 0.25f, position.y + size.y};  // Left = False
                Vector2 trueOut = {position.x + size.x * 0.75f, position.y + size.y};   // Right = True
                outputPorts.push_back(ConnectionPoint(falseOut, ConnectionPortType::FALSE_OUT, id, "False", {220, 53, 69, 255})); // Red for false
                outputPorts.push_back(ConnectionPoint(trueOut, ConnectionPortType::TRUE_OUT, id, "True", {25, 135, 84, 255})); // Green for true
            }
        } else {
            // Add single execution output port (bottom center)
            Vector2 outputPos = {position.x + size.x / 2, position.y + size.y};
            outputPorts.push_back(ConnectionPoint(outputPos, ConnectionPortType::EXECUTION_OUT, id, "Out", WHITE));
        }
    }
    
    // Special case for IF blocks - they have no input execution port but have outputs
    if (type == BlockType::IF) {
        // Clear input ports since IF blocks don't have execution input
        inputPorts.clear();
        
        // Add false (left) and true (right) output ports only
        Vector2 falseOut = {position.x, position.y + size.y / 2};  // Left = False
        Vector2 trueOut = {position.x + size.x, position.y + size.y / 2};   // Right = True
        outputPorts.clear();
        outputPorts.push_back(ConnectionPoint(falseOut, ConnectionPortType::FALSE_OUT, id, "False", {220, 53, 69, 255})); // Red for false
        outputPorts.push_back(ConnectionPoint(trueOut, ConnectionPortType::TRUE_OUT, id, "True", {25, 135, 84, 255})); // Green for true
    }
    
    // Add value input/output ports for specific block types
    switch (type) {
        case BlockType::IF:
            break; // IF blocks don't have value ports
        case BlockType::LOOP:
            break; // LOOP blocks don't have value ports
        case BlockType::VALUE:
        case BlockType::TRUE:
        case BlockType::FALSE: {
            // Add value output port
            Vector2 valueOut = {position.x + size.x, position.y + size.y / 2};
            outputPorts.push_back(ConnectionPoint(valueOut, ConnectionPortType::VALUE_OUT, id, "Value", primaryColor));
            break;
        }
        case BlockType::MOVE: {
            // Add direction and speed input ports
            Vector2 dirIn = {position.x, position.y + size.y * 0.4f};
            Vector2 speedIn = {position.x, position.y + size.y * 0.7f};
            inputPorts.push_back(ConnectionPoint(dirIn, ConnectionPortType::VALUE_IN, id, "Direction", {25, 135, 84, 255}));
            inputPorts.push_back(ConnectionPoint(speedIn, ConnectionPortType::VALUE_IN, id, "Speed", {25, 135, 84, 255}));
            break;
        }
        default:
            break;
    }
}

Rectangle ScriptBlock::getHeaderBounds() const {
    return Rectangle{position.x, position.y, size.x, size.y * 0.4f};
}

Rectangle ScriptBlock::getBodyBounds() const {
    float headerHeight = size.y * 0.4f;
    return Rectangle{position.x, position.y + headerHeight, size.x, size.y - headerHeight};
}

bool ScriptBlock::isPointInHeader(Vector2 point) const {
    Rectangle headerRect = getHeaderBounds();
    return CheckCollisionPointRec(point, headerRect);
}

void ScriptingEditor::drawBlockPalette(Rectangle bounds) {
    // Draw panel background
    DrawRectangleRec(bounds, UI::PANEL_BACKGROUND);
    DrawRectangleLinesEx(bounds, 1, UI::UI_PRIMARY);
    
    // Panel title
    Rectangle titleArea = {bounds.x + 5, bounds.y + 5, bounds.width - 10, 25};
    GuiLabel(titleArea, "Block Palette");
    
    // Content area
    Rectangle contentArea = {bounds.x + 5, bounds.y + 35, bounds.width - 10, bounds.height - 40};
    float yOffset = 5;
    
    // Draw each category
    for (const auto& category : _blockPalette) {
        std::string categoryName;
        switch (category.first) {
            case BlockCategory::EVENTS: categoryName = "Events"; break;
            case BlockCategory::ACTIONS: categoryName = "Actions"; break;
            case BlockCategory::CONDITIONS: categoryName = "Conditions"; break;
            case BlockCategory::MISC: categoryName = "Miscellaneous"; break;
        }
        
        drawPaletteCategory(categoryName, category.second, contentArea, yOffset);
    }
}

void ScriptingEditor::drawPaletteCategory(const std::string& title, const std::vector<BlockType>& blocks, 
                                        Rectangle bounds, float& yOffset) {
    // Category header
    Rectangle headerBounds = {bounds.x, bounds.y + yOffset, bounds.width, 20};
    GuiLabel(headerBounds, title.c_str());
    yOffset += 25;
    
    // Draw blocks in this category
    for (BlockType blockType : blocks) {
        ScriptBlock paletteBlock(_nextBlockId, blockType);
        paletteBlock.position = {bounds.x + 5, bounds.y + yOffset};
        paletteBlock.size = {bounds.width - 10, 35}; // Fit to palette width
        
        // Check if mouse is over this block for dragging
        Vector2 mousePos = _lastMousePos;
        bool isHovered = isPointInBlock(mousePos, paletteBlock);
        
        // Highlight if hovered
        if (isHovered) {
            DrawRectangleRec({paletteBlock.position.x, paletteBlock.position.y, 
                            paletteBlock.size.x, paletteBlock.size.y}, 
                           {paletteBlock.color.r, paletteBlock.color.g, paletteBlock.color.b, 150});
        }
        
        drawProfessionalBlock(paletteBlock, {0, 0});
        yOffset += 40;
    }
    
    yOffset += 10; // Space between categories
}

void ScriptingEditor::drawScriptCanvas(Rectangle bounds) {
    // Draw panel background
    DrawRectangleRec(bounds, Color{250, 250, 250, 255}); // Light gray canvas
    DrawRectangleLinesEx(bounds, 1, UI::UI_PRIMARY);
    
    // Check if an object is selected
    int selectedObjId = getSelectedObjectId();
    if (selectedObjId == -1) {
        // No object selected - draw dimmed overlay and message
        drawCanvasOverlay(bounds);
        return;
    }
    
    // Object is selected - draw the script canvas
    UI::SceneObjectInfo objInfo = getObjectInfo(selectedObjId);
    
    // Object title
    Rectangle titleArea = {bounds.x + 10, bounds.y + 10, bounds.width - 20, 25};
    std::string title = "Script for: " + objInfo.name;
    GuiLabel(titleArea, title.c_str());
    
    // Canvas area (with offset for title)
    Rectangle canvasArea = {bounds.x + 10, bounds.y + 40, bounds.width - 20, bounds.height - 50};
    
    // Draw grid background
    drawCanvasGrid(canvasArea);
    
    // Get current script for this object
    auto it = _objectScripts.find(selectedObjId);
    if (it != _objectScripts.end()) {
        // Draw all blocks in the script with proper ordering
        std::vector<ScriptBlock*> sortedBlocks;
        for (auto& block : it->second.blocks) {
            if (block.isOnCanvas) {
                sortedBlocks.push_back(&block);
            }
        }
        
        // Sort by canvas order
        std::sort(sortedBlocks.begin(), sortedBlocks.end(), 
                 [](const ScriptBlock* a, const ScriptBlock* b) {
                     return a->canvasOrder < b->canvasOrder;
                 });
        
        // Connections removed - modern port-based system will be implemented later
        
        // Draw all canvas blocks with professional styling
        for (const ScriptBlock* block : sortedBlocks) {
            drawProfessionalBlock(*block, _canvasOffset);
        }
    }
    
    // Draw canvas border
    DrawRectangleLinesEx(canvasArea, 1, UI::UI_SECONDARY);
}

void ScriptingEditor::drawSceneObjectPanel(Rectangle bounds) {
    // Draw panel background
    DrawRectangleRec(bounds, UI::PANEL_BACKGROUND);
    DrawRectangleLinesEx(bounds, 1, UI::UI_PRIMARY);
    
    // Panel title
    Rectangle titleArea = {bounds.x + 5, bounds.y + 5, bounds.width - 10, 25};
    GuiLabel(titleArea, "Scene Objects");
    
    // Content area
    Rectangle contentArea = {bounds.x + 5, bounds.y + 35, bounds.width - 10, bounds.height - 40};
    float yOffset = 5;
    
    // Get scene objects from the scene provider
    std::vector<UI::SceneObjectInfo> sceneObjects = getSceneObjects();
    int selectedObjId = getSelectedObjectId();
    
    // Draw each scene object
    for (const auto& obj : sceneObjects) {
        Rectangle objRect = {contentArea.x, contentArea.y + yOffset, contentArea.width, 30};
        
        // Check if this object is selected
        bool isSelected = (obj.id == selectedObjId);
        
        // Draw background for selected object
        if (isSelected) {
            DrawRectangleRec(objRect, UI::ACCENT_PRIMARY);
        } else {
            // Check if mouse is hovering over this object
            Vector2 mousePos = _currentMousePos;
            if (CheckCollisionPointRec(mousePos, objRect)) {
                DrawRectangleRec(objRect, Color{200, 200, 200, 100});
            }
        }
        
        // Draw object name
        Rectangle textRect = {objRect.x + 5, objRect.y + 5, objRect.width - 10, 20};
        GuiLabel(textRect, obj.name.c_str());
        
        // Check for click on this object
        if (CheckCollisionPointRec(_currentMousePos, objRect) && _leftMousePressed) {
            // Select this object
            selectObject(obj.id);
        }
        
        yOffset += 35;
        
        // Stop if we've gone past the visible area
        if (yOffset > contentArea.height) {
            break;
        }
    }
}

void ScriptingEditor::drawCanvasOverlay(Rectangle bounds) {
    // Draw dark transparent overlay
    DrawRectangleRec(bounds, Color{0, 0, 0, 100});
    
    // Draw centered message
    Rectangle messageArea = {
        bounds.x + bounds.width/2 - 200,
        bounds.y + bounds.height/2 - 10,
        400, 20
    };
    
    // Draw message background
    DrawRectangleRec(messageArea, UI::PANEL_BACKGROUND);
    DrawRectangleLinesEx(messageArea, 1, UI::UI_PRIMARY);
    
    GuiLabel(messageArea, "Select a scene object to start scripting.");
}

void ScriptingEditor::drawCanvasGrid(Rectangle bounds) {
    const float gridSize = 20.0f;
    Color gridColor = {220, 220, 220, 255};
    
    // Draw vertical lines
    for (float x = bounds.x; x < bounds.x + bounds.width; x += gridSize) {
        DrawLine(x, bounds.y, x, bounds.y + bounds.height, gridColor);
    }
    
    // Draw horizontal lines
    for (float y = bounds.y; y < bounds.y + bounds.height; y += gridSize) {
        DrawLine(bounds.x, y, bounds.x + bounds.width, y, gridColor);
    }
}

// Professional block drawing implementation
void ScriptingEditor::drawProfessionalBlock(const ScriptBlock& block, Vector2 offset) {
    Vector2 drawPos = {block.position.x + offset.x, block.position.y + offset.y};
    Rectangle blockRect = {drawPos.x, drawPos.y, block.size.x, block.size.y};
    
    // Draw shadow for depth using enhanced shadow system
    Rectangle shadowRect = {drawPos.x + UI::UI_SHADOW_OFFSET_MEDIUM, drawPos.y + UI::UI_SHADOW_OFFSET_MEDIUM, block.size.x, block.size.y};
    DrawRectangleRounded(shadowRect, UI::UI_BORDER_RADIUS_LARGE / block.size.x, 8, UI::SHADOW_MEDIUM);
    
    // Draw main block background with gradient effect
    DrawRectangleRounded(blockRect, UI::UI_BORDER_RADIUS_LARGE / block.size.x, 8, block.primaryColor);
    
    // Draw subtle gradient overlay
    Rectangle gradientRect = {drawPos.x, drawPos.y, block.size.x, block.size.y * 0.3f};
    Color lightOverlay = {255, 255, 255, 30};
    DrawRectangleRounded(gradientRect, UI::UI_BORDER_RADIUS_LARGE / block.size.x, 8, lightOverlay);
    
    // Draw border using enhanced theme colors
    Color borderColor = UI::PANEL_BORDER;
    if (block.isSelected) {
        borderColor = UI::ACCENT_PRIMARY; // Use accent primary for selection
    } else if (block.isHovered) {
        borderColor = UI::HOVER_BACKGROUND; // Use hover background for hover
    } else if (block.isDragging) {
        borderColor = UI::ACCENT_TERTIARY; // Use accent tertiary for dragging
    }
    DrawRectangleRoundedLinesEx(blockRect, UI::UI_BORDER_RADIUS_LARGE / block.size.x, 8, 2.0f, borderColor);
    
    // Draw header and body sections with consistent spacing
    Rectangle headerRect = {drawPos.x, drawPos.y, block.size.x, block.size.y * 0.4f};
    Rectangle bodyRect = {drawPos.x, drawPos.y + block.size.y * 0.4f, block.size.x, block.size.y * 0.6f};
    
    drawBlockHeader(block, headerRect, offset);
    if (block.isOnCanvas) {
        drawBlockBody(block, bodyRect, offset);
        drawBlockPorts(block, offset);
    }
    
    // Draw selection highlight if selected using enhanced theme
    if (block.isSelected) {
        Rectangle highlightRect = {drawPos.x - UI::UI_PADDING_SMALL, drawPos.y - UI::UI_PADDING_SMALL, 
                                  block.size.x + UI::UI_PADDING_MEDIUM, block.size.y + UI::UI_PADDING_MEDIUM};
        DrawRectangleRoundedLinesEx(highlightRect, UI::UI_BORDER_RADIUS_LARGE / block.size.x, 8, 3.0f, UI::GLOW_ACCENT);
    }
}

void ScriptingEditor::drawBlockHeader(const ScriptBlock& block, Rectangle headerRect, Vector2 offset) {
    // Draw header background
    DrawRectangleRounded(headerRect, UI::UI_BORDER_RADIUS_LARGE / block.size.x, 8, block.headerColor);
    
    // Draw title using enhanced font sizing
    Vector2 titleSize = MeasureTextEx(GetFontDefault(), block.title.c_str(), UI::UI_FONT_SIZE_LARGE, 1);
    Vector2 titlePos = {
        headerRect.x + (headerRect.width - titleSize.x) / 2,
        headerRect.y + UI::UI_PADDING_MEDIUM
    };
    
    // Title shadow using enhanced shadow system
    DrawTextEx(GetFontDefault(), block.title.c_str(), 
               Vector2{titlePos.x + 1, titlePos.y + 1}, UI::UI_FONT_SIZE_LARGE, 1, UI::SHADOW_LIGHT);
    // Title text using enhanced text colors
    DrawTextEx(GetFontDefault(), block.title.c_str(), titlePos, UI::UI_FONT_SIZE_LARGE, 1, UI::UI_TEXT_PRIMARY);
}

void ScriptingEditor::drawBlockBody(const ScriptBlock& block, Rectangle bodyRect, Vector2 offset) {
    // Draw body background (slightly lighter than header)
    Color bodyColor = block.secondaryColor;
    DrawRectangleRounded(bodyRect, UI::UI_BORDER_RADIUS_LARGE / block.size.x, 8, bodyColor);
    
    // Draw parameter summary or subtitle
    std::string paramText = block.isOnCanvas ? block.getParameterSummary() : block.subtitle;
    
    Vector2 paramSize = MeasureTextEx(GetFontDefault(), paramText.c_str(), UI::UI_FONT_SIZE_MEDIUM, 1);
    Vector2 paramPos = {
        bodyRect.x + (bodyRect.width - paramSize.x) / 2,
        bodyRect.y + (bodyRect.height - paramSize.y) / 2
    };
    
    // Parameter text using enhanced text colors
    DrawTextEx(GetFontDefault(), paramText.c_str(), paramPos, UI::UI_FONT_SIZE_MEDIUM, 1, UI::UI_TEXT_SECONDARY);
    
    // Draw parameter fields for specific block types
    if (block.isOnCanvas) {
        switch (block.type) {
            case BlockType::MOVE: {
                // Draw small directional arrow or vector indicator
                Vector2 center = {bodyRect.x + bodyRect.width - UI::UI_PADDING_LARGE, bodyRect.y + bodyRect.height / 2};
                DrawCircle(center.x, center.y, 6, UI::BLOCK_ACTION_SECONDARY);
                DrawTriangle(
                    {center.x - 3, center.y + 2},
                    {center.x + 3, center.y + 2},
                    {center.x, center.y - 3},
                    UI::BLOCK_ACTION
                );
                break;
            }
            case BlockType::ROTATE: {
                // Draw rotation indicator
                Vector2 center = {bodyRect.x + bodyRect.width - UI::UI_PADDING_LARGE, bodyRect.y + bodyRect.height / 2};
                DrawCircleLines(center.x, center.y, 6, UI::BLOCK_ACTION);
                DrawCircle(center.x + 4, center.y - 4, 2, UI::BLOCK_ACTION);
                break;
            }
            case BlockType::IF: {
                // Draw condition diamond
                Vector2 center = {bodyRect.x + bodyRect.width - UI::UI_PADDING_LARGE, bodyRect.y + bodyRect.height / 2};
                Vector2 points[4] = {
                    {center.x, center.y - 6},
                    {center.x + 6, center.y},
                    {center.x, center.y + 6},
                    {center.x - 6, center.y}
                };
                DrawTriangle(points[0], points[1], points[2], UI::BLOCK_CONDITION_SECONDARY);
                DrawTriangle(points[0], points[2], points[3], UI::BLOCK_CONDITION_SECONDARY);
                break;
            }
            case BlockType::LOOP: {
                // Draw loop indicator (circular arrow)
                Vector2 center = {bodyRect.x + bodyRect.width - UI::UI_PADDING_LARGE, bodyRect.y + bodyRect.height / 2};
                DrawCircleLines(center.x, center.y, 6, UI::BLOCK_CONDITION);
                // Draw small arrow tip
                DrawTriangle(
                    {center.x + 4, center.y - 4},
                    {center.x + 6, center.y - 2},
                    {center.x + 4, center.y},
                    UI::BLOCK_CONDITION
                );
                break;
            }
            default:
                break;
        }
    }
}

void ScriptingEditor::drawBlockPorts(const ScriptBlock& block, Vector2 offset) {
    // Update port positions based on current block position
    ScriptBlock* mutableBlock = const_cast<ScriptBlock*>(&block);
    mutableBlock->setupConnectionPorts();
    
    // Draw input ports
    for (const auto& port : block.inputPorts) {
        Vector2 portPos = {port.position.x + offset.x, port.position.y + offset.y};
        
        // Draw port circle using enhanced theme colors
        DrawCircle(portPos.x, portPos.y, 6, port.portColor);
        DrawCircleLines(portPos.x, portPos.y, 6, UI::UI_TEXT_PRIMARY);
        
        // Draw port type indicator
        switch (port.type) {
            case ConnectionPortType::EXECUTION_IN:
                // Draw triangle pointing right
                DrawTriangle(
                    {portPos.x - 3, portPos.y - 3},
                    {portPos.x - 3, portPos.y + 3},
                    {portPos.x + 3, portPos.y},
                    UI::UI_TEXT_PRIMARY
                );
                break;
            case ConnectionPortType::VALUE_IN:
                // Draw small square
                DrawRectangle(portPos.x - 2, portPos.y - 2, 4, 4, UI::UI_TEXT_PRIMARY);
                break;
            default:
                break;
        }
        
        // Draw port label if space allows using enhanced typography
        if (!port.label.empty() && block.size.x > 120) {
            Vector2 labelSize = MeasureTextEx(GetFontDefault(), port.label.c_str(), UI::UI_FONT_SIZE_SMALL, 1);
            Vector2 labelPos = {portPos.x - labelSize.x - UI::UI_PADDING_MEDIUM, portPos.y - labelSize.y / 2};
            DrawTextEx(GetFontDefault(), port.label.c_str(), labelPos, UI::UI_FONT_SIZE_SMALL, 1, UI::UI_TEXT_TERTIARY);
        }
    }
    
    // Draw output ports
    for (const auto& port : block.outputPorts) {
        Vector2 portPos = {port.position.x + offset.x, port.position.y + offset.y};
        
        // Draw port circle using enhanced theme colors
        DrawCircle(portPos.x, portPos.y, 6, port.portColor);
        DrawCircleLines(portPos.x, portPos.y, 6, UI::UI_TEXT_PRIMARY);
        
        // Draw port type indicator
        switch (port.type) {
            case ConnectionPortType::EXECUTION_OUT:
                // Draw triangle pointing down
                DrawTriangle(
                    {portPos.x - 3, portPos.y - 3},
                    {portPos.x + 3, portPos.y - 3},
                    {portPos.x, portPos.y + 3},
                    UI::UI_TEXT_PRIMARY
                );
                break;
            case ConnectionPortType::TRUE_OUT:
                // Draw checkmark
                DrawLineEx({portPos.x - 3, portPos.y}, {portPos.x - 1, portPos.y + 2}, 2, UI::SUCCESS);
                DrawLineEx({portPos.x - 1, portPos.y + 2}, {portPos.x + 3, portPos.y - 2}, 2, UI::SUCCESS);
                break;
            case ConnectionPortType::FALSE_OUT:
                // Draw X
                DrawLineEx({portPos.x - 2, portPos.y - 2}, {portPos.x + 2, portPos.y + 2}, 2, UI::ERROR);
                DrawLineEx({portPos.x - 2, portPos.y + 2}, {portPos.x + 2, portPos.y - 2}, 2, UI::ERROR);
                break;
            case ConnectionPortType::VALUE_OUT:
                // Draw small circle
                DrawCircle(portPos.x, portPos.y, 2, UI::UI_TEXT_PRIMARY);
                break;
            default:
                break;
        }
        
        // Draw port label if space allows
        if (!port.label.empty() && block.size.x > 120) {
            Vector2 labelSize = MeasureTextEx(GetFontDefault(), port.label.c_str(), 10, 1);
            Vector2 labelPos = {portPos.x + 10, portPos.y - labelSize.y / 2};
            DrawTextEx(GetFontDefault(), port.label.c_str(), labelPos, 10, 1, {80, 80, 80, 255});
        }
    }
}

// Mouse and drag handling implementation
void ScriptingEditor::handleMouseInput(Vector2 mousePos, bool mousePressed, bool mouseReleased, bool rightClick) {
    // Handle right-click first
    if (rightClick && !_rightClickHandled) {
        handleRightClick(mousePos);
        _rightClickHandled = true;
    } else if (!rightClick) {
        _rightClickHandled = false;
    }
    
    // Handle left-click for block selection (when not right-clicking)
    if (mousePressed && !rightClick && !_isMouseDragging) {
        // Close context menu if open
        if (_showContextMenu) {
            closeContextMenu();
        }
        
        // Check for block selection
        if (isPositionInCanvas(mousePos)) {
            ScriptBlock* clickedBlock = getCanvasBlockAtPosition(mousePos);
            if (clickedBlock) {
                selectBlock(clickedBlock);
            } else {
                deselectAllBlocks();
            }
        }
    }
    
    // Handle drag and drop state machine
    if (mousePressed && !_isMouseDragging) {
        startDragOperation(mousePos);
    } else if (_isMouseDragging && !mouseReleased) {
        updateDragOperation(mousePos);
    } else if (mouseReleased && _isMouseDragging) {
        endDragOperation(mousePos);
    }
}

void ScriptingEditor::startDragOperation(Vector2 mousePos) {
    // Check if palette block is clicked
    if (isPositionInPalette(mousePos)) {
        BlockType blockType = getPaletteBlockTypeAtPosition(mousePos);
        if (blockType != BlockType::INVALID) {
            _isMouseDragging = true;
            _isDraggingFromPalette = true;
            _isDraggingCanvasBlock = false;
            _draggedBlockType = blockType;
            _dragStartPos = mousePos;
            std::cout << "[ScriptingEditor] Started dragging from palette: " << getBlockLabel(blockType) << std::endl;
            return;
        }
    }
    
    // Check if canvas block is clicked (only if object is selected)
    int selectedObjId = getSelectedObjectId();
    if (selectedObjId != -1 && isPositionInCanvas(mousePos)) {
        ScriptBlock* canvasBlock = getCanvasBlockAtPosition(mousePos);
        if (canvasBlock) {
            _isMouseDragging = true;
            _isDraggingFromPalette = false;
            _isDraggingCanvasBlock = true;
            _draggedCanvasBlock = canvasBlock;
            _dragStartPos = mousePos;
            _dragOffset = {mousePos.x - canvasBlock->position.x, mousePos.y - canvasBlock->position.y};
            canvasBlock->isDragging = true;
            std::cout << "[ScriptingEditor] Started dragging canvas block: " << canvasBlock->title << std::endl;
            return;
        }
    }
}

void ScriptingEditor::updateDragOperation(Vector2 mousePos) {
    if (!_isMouseDragging) return;
    
    if (_isDraggingCanvasBlock && _draggedCanvasBlock) {
        Vector2 oldPos = _draggedCanvasBlock->position;
        
        // Update canvas block position
        _draggedCanvasBlock->position = {
            mousePos.x - _dragOffset.x,
            mousePos.y - _dragOffset.y
        };
        
        // Position updated - no legacy connection handling needed
    }
    // For palette dragging, the preview is handled in draw()
}

void ScriptingEditor::endDragOperation(Vector2 mousePos) {
    if (!_isMouseDragging) return;
    
    if (_isDraggingFromPalette) {
        // Check if dropping on canvas
        int selectedObjId = getSelectedObjectId();
        if (selectedObjId != -1 && isPositionInCanvas(mousePos)) {
            Vector2 canvasPos = {mousePos.x - 60, mousePos.y - 20}; // Center block on mouse
            addBlockToCanvas(_draggedBlockType, canvasPos);
            std::cout << "[ScriptingEditor] Added block to canvas: " << getBlockLabel(_draggedBlockType) << std::endl;
        }
    } else if (_isDraggingCanvasBlock && _draggedCanvasBlock) {
        // Check if block was dragged out of canvas
        if (!isPositionInCanvas(mousePos)) {
            // Remove block when dragged out of canvas
            int blockId = _draggedCanvasBlock->id;
            removeBlockFromCanvas(blockId);
            std::cout << "[ScriptingEditor] Removed block from canvas" << std::endl;
        }
        
        // Reset drag state for the block
        _draggedCanvasBlock->isDragging = false;
        _draggedCanvasBlock = nullptr;
    }
    
    // Reset drag state
    resetDragState();
}

void ScriptingEditor::resetDragState() {
    _isMouseDragging = false;
    _isDraggingFromPalette = false;
    _isDraggingCanvasBlock = false;
    _draggedCanvasBlock = nullptr;
    _dragStartPos = {0, 0};
    _dragOffset = {0, 0};
}

ScriptBlock* ScriptingEditor::getCanvasBlockAtPosition(Vector2 pos) {
    int selectedObjId = getSelectedObjectId();
    if (selectedObjId == -1) return nullptr;
    
    auto it = _objectScripts.find(selectedObjId);
    if (it == _objectScripts.end()) return nullptr;
    
    // Check blocks in reverse order (top-most first)
    for (auto rit = it->second.blocks.rbegin(); rit != it->second.blocks.rend(); ++rit) {
        if (rit->isOnCanvas && isPointInBlock(pos, *rit, _canvasOffset)) {
            return &(*rit);
        }
    }
    
    return nullptr;
}

bool ScriptingEditor::isPointInBlock(Vector2 point, const ScriptBlock& block, Vector2 offset) {
    Vector2 blockPos = {block.position.x + offset.x, block.position.y + offset.y};
    Rectangle blockRect = {blockPos.x, blockPos.y, block.size.x, block.size.y};
    return CheckCollisionPointRec(point, blockRect);
}

// Helper methods
ScriptBlock ScriptingEditor::createBlockFromType(BlockType type, Vector2 position) {
    return ScriptBlock(_nextBlockId++, type, position);
}

void ScriptingEditor::addBlockToCanvas(BlockType type, Vector2 position) {
    int selectedObjId = getSelectedObjectId();
    if (selectedObjId == -1) return;
    
    // Ensure script exists for this object
    if (_objectScripts.find(selectedObjId) == _objectScripts.end()) {
        _objectScripts[selectedObjId] = VisualScript(selectedObjId);
    }
    
    ScriptBlock newBlock = createBlockFromType(type, position);
    newBlock.isOnCanvas = true;
    newBlock.canvasOrder = _nextCanvasOrder++;
    _objectScripts[selectedObjId].blocks.push_back(newBlock);
    
    std::cout << "[ScriptingEditor] Added block '" << newBlock.title << "' to object " << selectedObjId << std::endl;
}

void ScriptingEditor::removeBlockFromCanvas(int blockId) {
    int selectedObjId = getSelectedObjectId();
    if (selectedObjId == -1) return;
    
    auto it = _objectScripts.find(selectedObjId);
    if (it == _objectScripts.end()) return;
    
    auto& blocks = it->second.blocks;
    blocks.erase(
        std::remove_if(blocks.begin(), blocks.end(),
            [blockId](const ScriptBlock& block) { return block.id == blockId; }),
        blocks.end()
    );
    
    std::cout << "[ScriptingEditor] Removed block " << blockId << " from object " << selectedObjId << std::endl;
}

VisualScript& ScriptingEditor::getCurrentScript() {
    int selectedObjId = getSelectedObjectId();
    
    // Ensure script exists for this object
    if (_objectScripts.find(selectedObjId) == _objectScripts.end()) {
        _objectScripts[selectedObjId] = VisualScript(selectedObjId);
    }
    
    return _objectScripts[selectedObjId];
}

std::string ScriptingEditor::getBlockLabel(BlockType type) const {
    ScriptBlock tempBlock(0, type);
    return tempBlock.title;
}

Color ScriptingEditor::getBlockColor(BlockType type) const {
    ScriptBlock tempBlock(0, type);
    return tempBlock.primaryColor;
}

void ScriptingEditor::handleObjectSelection(int objectId) {
    _selectedObjectId = objectId;
    selectObject(objectId); // Forward to scene provider
    std::cout << "[ScriptingEditor] Selected object for scripting: " << objectId << std::endl;
}

// New method implementations for enhanced functionality

void ScriptingEditor::handleRightClick(Vector2 mousePos) {
    // Check if right-clicking on a canvas block
    int selectedObjId = getSelectedObjectId();
    if (selectedObjId == -1) return;
    
    if (isPositionInCanvas(mousePos)) {
        ScriptBlock* block = getCanvasBlockAtPosition(mousePos);
        if (block) {
            openContextMenu(block, mousePos);
        } else {
            // Close context menu if clicking on empty canvas
            closeContextMenu();
        }
    } else {
        // Close context menu if clicking outside canvas
        closeContextMenu();
    }
}


Vector2 ScriptingEditor::getNextCanvasPosition() {
    // Calculate the next position for a new block on canvas
    Rectangle canvasArea = {_leftPanelWidth + _panelPadding + 20, 60, 1000, 1000};
    
    int selectedObjId = getSelectedObjectId();
    if (selectedObjId == -1) {
        return {canvasArea.x, canvasArea.y};
    }
    
    auto it = _objectScripts.find(selectedObjId);
    if (it == _objectScripts.end() || it->second.blocks.empty()) {
        return {canvasArea.x, canvasArea.y};
    }
    
    // Find the lowest positioned block
    float maxY = canvasArea.y;
    for (const auto& block : it->second.blocks) {
        if (block.isOnCanvas) {
            maxY = std::max(maxY, block.position.y + block.size.y);
        }
    }
    
    return {canvasArea.x, maxY + _blockSpacing};
}

void ScriptingEditor::reorderCanvasBlocks() {
    int selectedObjId = getSelectedObjectId();
    if (selectedObjId == -1) return;
    
    auto it = _objectScripts.find(selectedObjId);
    if (it == _objectScripts.end()) return;
    
    // Get all canvas blocks and sort by Y position
    std::vector<ScriptBlock*> canvasBlocks;
    for (auto& block : it->second.blocks) {
        if (block.isOnCanvas) {
            canvasBlocks.push_back(&block);
        }
    }
    
    std::sort(canvasBlocks.begin(), canvasBlocks.end(),
             [](const ScriptBlock* a, const ScriptBlock* b) {
                 return a->position.y < b->position.y;
             });
    
    // Reassign canvas orders and snap positions
    Rectangle canvasArea = {_leftPanelWidth + _panelPadding + 20, 60, 1000, 1000};
    float currentY = canvasArea.y;
    
    for (int i = 0; i < canvasBlocks.size(); i++) {
        canvasBlocks[i]->canvasOrder = i;
        canvasBlocks[i]->position = {canvasArea.x, currentY};
        currentY += canvasBlocks[i]->size.y + _blockSpacing;
    }
}


void ScriptingEditor::openConfigDialog(ScriptBlock* block) {
    _showConfigDialog = true;
    _configuredBlock = block;
    std::cout << "[ScriptingEditor] Config dialog opened, showConfigDialog=" << _showConfigDialog << std::endl;
}

void ScriptingEditor::closeConfigDialog() {
    _showConfigDialog = false;
    _configuredBlock = nullptr;
}

void ScriptingEditor::applyBlockConfiguration() {
    // Configuration is applied in real-time in the dialog
    closeConfigDialog();
}

void ScriptingEditor::drawConfigDialog() {
    if (!_configuredBlock) return;
    
    // Draw dark overlay over entire screen
    float screenWidth = GetScreenWidth();
    float screenHeight = GetScreenHeight();
    DrawRectangle(0, 0, screenWidth, screenHeight, Color{0, 0, 0, 150});
    
    // Center the dialog on screen
    float dialogWidth = 400;
    float dialogHeight = 350;
    Rectangle dialogBounds = {
        (screenWidth - dialogWidth) / 2,
        (screenHeight - dialogHeight) / 2,
        dialogWidth,
        dialogHeight
    };
    
    // Draw dialog background with shadow
    Rectangle shadowRect = {dialogBounds.x + 5, dialogBounds.y + 5, dialogBounds.width, dialogBounds.height};
    DrawRectangleRec(shadowRect, Color{0, 0, 0, 100});
    
    // Draw dialog background
    DrawRectangleRec(dialogBounds, Color{255, 255, 255, 255});
    DrawRectangleLinesEx(dialogBounds, 3, UI::UI_PRIMARY);
    
    // Title
    Rectangle titleArea = {dialogBounds.x + 10, dialogBounds.y + 10, dialogBounds.width - 20, 25};
    std::string title = "Configure: " + _configuredBlock->label;
    GuiLabel(titleArea, title.c_str());
    
    // Configuration fields based on block type
    float yOffset = 50;
    
    switch (_configuredBlock->type) {
        case BlockType::ON_KEY_PRESS: {
            Rectangle keyLabel = {dialogBounds.x + 10, dialogBounds.y + yOffset, dialogBounds.width - 20, 20};
            GuiLabel(keyLabel, "Key:");
            
            Rectangle keyField = {dialogBounds.x + 10, dialogBounds.y + yOffset + 20, dialogBounds.width - 20, 25};
            static int keyIndex = 0;
            const char* keys[] = {"Space", "A", "W", "S", "D", "Up", "Down", "Left", "Right", "Enter"};
            keyIndex = GuiComboBox(keyField, "Space;A;W;S;D;Up;Down;Left;Right;Enter", &keyIndex);
            _configuredBlock->config.stringParams["key"] = keys[keyIndex];
            break;
        }
        
        case BlockType::MOVE: {
            Rectangle speedLabel = {dialogBounds.x + 10, dialogBounds.y + yOffset, dialogBounds.width - 20, 20};
            GuiLabel(speedLabel, "Speed:");
            
            Rectangle speedField = {dialogBounds.x + 10, dialogBounds.y + yOffset + 20, dialogBounds.width - 20, 25};
            static char speedText[32] = "1.0";
            GuiTextBox(speedField, speedText, 32, true);
            _configuredBlock->config.floatParams["speed"] = std::stof(speedText);
            
            yOffset += 50;
            
            Rectangle dirLabel = {dialogBounds.x + 10, dialogBounds.y + yOffset, dialogBounds.width - 20, 20};
            GuiLabel(dirLabel, "Direction (X, Y, Z):");
            
            static char xText[16] = "1.0", yText[16] = "0.0", zText[16] = "0.0";
            float fieldWidth = (dialogBounds.width - 40) / 3;
            
            Rectangle xField = {dialogBounds.x + 10, dialogBounds.y + yOffset + 20, fieldWidth, 25};
            Rectangle yField = {dialogBounds.x + 15 + fieldWidth, dialogBounds.y + yOffset + 20, fieldWidth, 25};
            Rectangle zField = {dialogBounds.x + 20 + 2*fieldWidth, dialogBounds.y + yOffset + 20, fieldWidth, 25};
            
            GuiTextBox(xField, xText, 16, true);
            GuiTextBox(yField, yText, 16, true);
            GuiTextBox(zField, zText, 16, true);
            
            _configuredBlock->config.vectorParams["direction"] = {std::stof(xText), std::stof(yText), std::stof(zText)};
            break;
        }
        
        case BlockType::ROTATE: {
            Rectangle speedLabel = {dialogBounds.x + 10, dialogBounds.y + yOffset, dialogBounds.width - 20, 20};
            GuiLabel(speedLabel, "Speed (degrees/second):");
            
            Rectangle speedField = {dialogBounds.x + 10, dialogBounds.y + yOffset + 20, dialogBounds.width - 20, 25};
            static char rotSpeedText[32] = "90.0";
            GuiTextBox(speedField, rotSpeedText, 32, true);
            _configuredBlock->config.floatParams["speed"] = std::stof(rotSpeedText);
            break;
        }
        
        case BlockType::DELAY: {
            Rectangle durationLabel = {dialogBounds.x + 10, dialogBounds.y + yOffset, dialogBounds.width - 20, 20};
            GuiLabel(durationLabel, "Duration (seconds):");
            
            Rectangle durationField = {dialogBounds.x + 10, dialogBounds.y + yOffset + 20, dialogBounds.width - 20, 25};
            static char durationText[32] = "1.0";
            GuiTextBox(durationField, durationText, 32, true);
            _configuredBlock->config.floatParams["duration"] = std::stof(durationText);
            break;
        }
        
        case BlockType::LOG: {
            Rectangle messageLabel = {dialogBounds.x + 10, dialogBounds.y + yOffset, dialogBounds.width - 20, 20};
            GuiLabel(messageLabel, "Message:");
            
            Rectangle messageField = {dialogBounds.x + 10, dialogBounds.y + yOffset + 20, dialogBounds.width - 20, 25};
            static char messageText[128] = "Debug message";
            GuiTextBox(messageField, messageText, 128, true);
            _configuredBlock->config.stringParams["message"] = messageText;
            break;
        }
        
        case BlockType::LOOP: {
            Rectangle iterLabel = {dialogBounds.x + 10, dialogBounds.y + yOffset, dialogBounds.width - 20, 20};
            GuiLabel(iterLabel, "Number of iterations:");
            
            Rectangle iterField = {dialogBounds.x + 10, dialogBounds.y + yOffset + 20, dialogBounds.width - 20, 25};
            static char iterText[32] = "5";
            GuiTextBox(iterField, iterText, 32, true);
            _configuredBlock->config.floatParams["iterations"] = std::stof(iterText);
            break;
        }
        
        default:
            Rectangle noConfigLabel = {dialogBounds.x + 10, dialogBounds.y + yOffset, dialogBounds.width - 20, 20};
            GuiLabel(noConfigLabel, "No configuration options for this block.");
            break;
    }
    
    // Buttons
    Rectangle applyButton = {dialogBounds.x + dialogBounds.width - 180, dialogBounds.y + dialogBounds.height - 40, 80, 30};
    Rectangle cancelButton = {dialogBounds.x + dialogBounds.width - 90, dialogBounds.y + dialogBounds.height - 40, 80, 30};
    
    if (GuiButton(applyButton, "Apply")) {
        applyBlockConfiguration();
    }
    
    if (GuiButton(cancelButton, "Cancel")) {
        closeConfigDialog();
    }
}

// Additional helper methods for improved drag and drop

void ScriptingEditor::updateCanvasBounds(Rectangle mainViewArea) {
    _canvasBounds = {
        mainViewArea.x + _leftPanelWidth + _panelPadding + 10,
        mainViewArea.y + 40,
        mainViewArea.width - _leftPanelWidth - _panelPadding - 20,
        mainViewArea.height - 50
    };
}

bool ScriptingEditor::isPositionInCanvas(Vector2 pos) {
    return CheckCollisionPointRec(pos, _canvasBounds);
}

bool ScriptingEditor::isPositionInPalette(Vector2 pos) {
    Rectangle paletteBounds = getPaletteBounds();
    return CheckCollisionPointRec(pos, paletteBounds);
}

Rectangle ScriptingEditor::getPaletteBounds() {
    return Rectangle{0, 0, _leftPanelWidth, 1000}; // Approximate palette area
}

BlockType ScriptingEditor::getPaletteBlockTypeAtPosition(Vector2 pos) {
    if (!isPositionInPalette(pos)) {
        return BlockType::INVALID;
    }
    
    float yOffset = 70; // Start after title
    for (const auto& category : _blockPalette) {
        yOffset += 25; // Category header
        for (BlockType blockType : category.second) {
            Rectangle blockBounds = {5, yOffset, _leftPanelWidth - 10, 35};
            if (CheckCollisionPointRec(pos, blockBounds)) {
                return blockType;
            }
            yOffset += 40;
        }
        yOffset += 10; // Space between categories
    }
    
    return BlockType::INVALID;
}











// Context menu implementation
void ScriptingEditor::openContextMenu(ScriptBlock* block, Vector2 position) {
    _showContextMenu = true;
    _contextMenuPosition = position;
    _contextMenuBlock = block;
    std::cout << "[ScriptingEditor] Opened context menu for block: " << block->title << std::endl;
}

void ScriptingEditor::closeContextMenu() {
    _showContextMenu = false;
    _contextMenuBlock = nullptr;
}

void ScriptingEditor::handleContextMenuAction(const std::string& action) {
    if (!_contextMenuBlock) return;
    
    if (action == "Edit") {
        editBlock(_contextMenuBlock);
    } else if (action == "Duplicate") {
        duplicateBlock(_contextMenuBlock);
    } else if (action == "Delete") {
        deleteBlock(_contextMenuBlock);
    }
    
    closeContextMenu();
}

void ScriptingEditor::duplicateBlock(ScriptBlock* block) {
    if (!block) return;
    
    int selectedObjId = getSelectedObjectId();
    if (selectedObjId == -1) return;
    
    // Create a new block of the same type
    Vector2 newPos = {block->position.x + 20, block->position.y + 20}; // Offset slightly
    ScriptBlock newBlock = createBlockFromType(block->type, newPos);
    
    // Copy configuration
    newBlock.config = block->config;
    newBlock.isOnCanvas = true;
    newBlock.canvasOrder = _nextCanvasOrder++;
    
    // Add to canvas
    auto it = _objectScripts.find(selectedObjId);
    if (it != _objectScripts.end()) {
        it->second.blocks.push_back(newBlock);
        std::cout << "[ScriptingEditor] Duplicated block: " << block->title << std::endl;
    }
}

void ScriptingEditor::deleteBlock(ScriptBlock* block) {
    if (!block) return;
    
    // Connection cleanup removed - modern port-based system will handle this
    
    // Remove the block from canvas
    removeBlockFromCanvas(block->id);
    
    std::cout << "[ScriptingEditor] Deleted block: " << block->title << std::endl;
}

void ScriptingEditor::editBlock(ScriptBlock* block) {
    if (block) {
        std::cout << "[ScriptingEditor] Opening config dialog for block: " << block->title << std::endl;
        openConfigDialog(block);
    }
}

void ScriptingEditor::selectBlock(ScriptBlock* block) {
    // Deselect all blocks first
    deselectAllBlocks();
    
    if (block) {
        block->isSelected = true;
        _selectedBlock = block;
        std::cout << "[ScriptingEditor] Selected block: " << block->title << std::endl;
    }
}

void ScriptingEditor::deselectAllBlocks() {
    int selectedObjId = getSelectedObjectId();
    if (selectedObjId == -1) return;
    
    auto it = _objectScripts.find(selectedObjId);
    if (it != _objectScripts.end()) {
        for (auto& block : it->second.blocks) {
            block.isSelected = false;
        }
    }
    _selectedBlock = nullptr;
}

void ScriptingEditor::updateBlockHover(Vector2 mousePos) {
    int selectedObjId = getSelectedObjectId();
    if (selectedObjId == -1) return;
    
    auto it = _objectScripts.find(selectedObjId);
    if (it != _objectScripts.end()) {
        for (auto& block : it->second.blocks) {
            if (block.isOnCanvas) {
                bool wasHovered = block.isHovered;
                block.isHovered = isPointInBlock(mousePos, block, _canvasOffset);
                
                // Update hover state
                if (block.isHovered && !wasHovered) {
                    _hoveredBlock = &block;
                } else if (!block.isHovered && wasHovered) {
                    _hoveredBlock = nullptr;
                }
            }
        }
    }
}

ScriptBlock* ScriptingEditor::getBlockAtPosition(Vector2 pos) {
    return getCanvasBlockAtPosition(pos);
}

void ScriptingEditor::drawContextMenu() {
    if (!_showContextMenu || !_contextMenuBlock) return;
    
    const float menuWidth = 140;
    const float menuHeight = 110;
    const float itemHeight = 30;
    const float padding = 8.0f; // UI::UI_PADDING_MEDIUM
    
    // Adjust position to stay within screen bounds
    Vector2 menuPos = _contextMenuPosition;
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    if (menuPos.x + menuWidth > screenWidth) menuPos.x = screenWidth - menuWidth;
    if (menuPos.y + menuHeight > screenHeight) menuPos.y = screenHeight - menuHeight;
    if (menuPos.x < 0) menuPos.x = 0;
    if (menuPos.y < 0) menuPos.y = 0;
    
    Rectangle menuBounds = {menuPos.x, menuPos.y, menuWidth, menuHeight};
    
    // Draw shadow first
    Rectangle shadowBounds = {menuPos.x + 4, menuPos.y + 4, menuWidth, menuHeight};
    DrawRectangleRec(shadowBounds, {0, 0, 0, 80});
    
    // Draw menu background with dark theme colors
    DrawRectangleRec(menuBounds, UI::PANEL_BACKGROUND);
    DrawRectangleLinesEx(menuBounds, 1, UI::PANEL_BORDER);
    
    // Draw menu items with enhanced styling
    Rectangle editRect = {menuPos.x + padding, menuPos.y + padding, menuWidth - 2*padding, itemHeight};
    Rectangle duplicateRect = {menuPos.x + padding, menuPos.y + padding + itemHeight + 5, menuWidth - 2*padding, itemHeight};
    Rectangle deleteRect = {menuPos.x + padding, menuPos.y + padding + 2*(itemHeight + 5), menuWidth - 2*padding, itemHeight};
    
    // Check if Edit option should be enabled (only for blocks with parameters)
    bool canEdit = _contextMenuBlock && (_contextMenuBlock->config.floatParams.size() > 0 || 
                                        _contextMenuBlock->config.vectorParams.size() > 0 || 
                                        _contextMenuBlock->config.stringParams.size() > 0 || 
                                        _contextMenuBlock->config.boolParams.size() > 0);
    
    // Enhanced menu item drawing with hover effects
    Vector2 mousePos = GetMousePosition();
    
    // Edit button
    bool editHovered = CheckCollisionPointRec(mousePos, editRect);
    Color editBgColor = canEdit ? (editHovered ? UI::ACCENT_PRIMARY : UI::UI_TERTIARY) : UI::UI_TEXT_TERTIARY;
    Color editTextColor = canEdit ? UI::UI_TEXT_PRIMARY : UI::UI_TEXT_TERTIARY;
    
    DrawRectangleRec(editRect, editBgColor);
    DrawRectangleLinesEx(editRect, 1, UI::PANEL_BORDER);
    DrawText("Edit", editRect.x + 10, editRect.y + 8, 12, editTextColor);
    
    if (canEdit && editHovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        handleContextMenuAction("Edit");
    }
    
    // Duplicate button
    bool duplicateHovered = CheckCollisionPointRec(mousePos, duplicateRect);
    Color duplicateBgColor = duplicateHovered ? UI::ACCENT_SECONDARY : UI::UI_TERTIARY;
    
    DrawRectangleRec(duplicateRect, duplicateBgColor);
    DrawRectangleLinesEx(duplicateRect, 1, UI::PANEL_BORDER);
    DrawText("Duplicate", duplicateRect.x + 10, duplicateRect.y + 8, 12, UI::UI_TEXT_PRIMARY);
    
    if (duplicateHovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        handleContextMenuAction("Duplicate");
    }
    
    // Delete button
    bool deleteHovered = CheckCollisionPointRec(mousePos, deleteRect);
    Color deleteBgColor = deleteHovered ? UI::ACCENT_DANGER : UI::UI_TERTIARY;
    
    DrawRectangleRec(deleteRect, deleteBgColor);
    DrawRectangleLinesEx(deleteRect, 1, UI::PANEL_BORDER);
    DrawText("Delete", deleteRect.x + 10, deleteRect.y + 8, 12, UI::UI_TEXT_PRIMARY);
    
    if (deleteHovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        handleContextMenuAction("Delete");
    }
}
