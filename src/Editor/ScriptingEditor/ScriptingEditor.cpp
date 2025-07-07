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
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>

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
    initializeBlockConfigTemplates();
    _initialized = true;
    
    std::cout << "[ScriptingEditor] Initialized successfully" << std::endl;
}

void ScriptingEditor::update(input::IHandlerBase &inputHandler) {
    if (!_initialized) {
        return;
    }
    
    Vector2D mousePosD = inputHandler.getCursorCoords();
    Vector2 mousePos = {(float)mousePosD.x, (float)mousePosD.y};
    bool mousePressed = IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    bool mouseReleased = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    bool rightClick = IsMouseButtonPressed(MOUSE_BUTTON_RIGHT);
    
    _lastMousePos = _currentMousePos;
    _currentMousePos = mousePos;
    _leftMousePressed = mousePressed;
    _leftMouseReleased = mouseReleased;
    
    updateBlockHover(mousePos);
    handleMouseInput(mousePos, mousePressed, mouseReleased, rightClick);
}

void ScriptingEditor::draw(Rectangle mainViewArea) {
    if (!_initialized) {
        return;
    }
    
    updateCanvasBounds(mainViewArea);
    DrawRectangleRec(mainViewArea, UI::BACKGROUND);
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
    
    drawBlockPalette(leftPanel);
    drawScriptCanvas(canvasPanel);
    drawSceneObjectPanel(rightPanel);
    
    if (_isDraggingFromPalette) {
        ScriptBlock previewBlock(0, _draggedBlockType, _currentMousePos);
        previewBlock.primaryColor.a = 150;
        drawProfessionalBlock(previewBlock, {-previewBlock.size.x/2, -previewBlock.size.y/2});
    }
    
    if (_showConfigDialog) {
        drawConfigDialog();
    }
    
    if (_connectionDrag.isDragging) {
        drawConnectionPreview();
    }
    
    if (_contextMenu.isVisible) {
        drawContextMenu();
    }
    
    DrawRectangleLinesEx(mainViewArea, 1, UI::UI_PRIMARY);
}

void ScriptingEditor::setupEventHandlers() {
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
            clearAllScripts();
            std::cout << "[ScriptingEditor] New script project created" << std::endl;
            break;
        case UI::EditorEventType::FILE_SAVE:
            saveCurrentScript();
            std::cout << "[ScriptingEditor] Scripts saved" << std::endl;
            break;
        case UI::EditorEventType::FILE_OPEN:
            if (!filepath.empty()) {
                loadScriptFromFile(filepath);
            }
            std::cout << "[ScriptingEditor] Script project loaded" << std::endl;
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
        
        // Draw connections first (behind blocks)
        drawConnections(it->second);
        
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
    DrawRectangleRoundedLines(blockRect, UI::UI_BORDER_RADIUS_LARGE / block.size.x, 8, 2.0f, borderColor);
    
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
        DrawRectangleRoundedLines(highlightRect, UI::UI_BORDER_RADIUS_LARGE / block.size.x, 8, 3.0f, UI::GLOW_ACCENT);
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
    
    // Update context menu if visible (handles hovering and clicking)
    if (_contextMenu.isVisible) {
        updateContextMenu(mousePos);
    }
    
    // Update connection system (handles connection creation)
    updateConnectionSystem(mousePos, mousePressed, mouseReleased);
    
    // Handle left-click for block selection (when not right-clicking and not making connections)
    if (mousePressed && !rightClick && !_isMouseDragging && !_connectionDrag.isDragging) {
        // Close context menu if clicking elsewhere
        if (_contextMenu.isVisible) {
            hideContextMenu();
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
    
    // Load current block configuration into field states
    loadFieldStatesFromBlock(block);
    
    std::cout << "[ScriptingEditor] Config dialog opened for block type " 
              << static_cast<int>(block->type) << std::endl;
}

void ScriptingEditor::closeConfigDialog() {
    _showConfigDialog = false;
    _configuredBlock = nullptr;
    resetFieldStates();
}

void ScriptingEditor::applyBlockConfiguration() {
    if (_configuredBlock) {
        // Apply field states to the block
        applyFieldStatesToBlock(_configuredBlock);
        std::cout << "[ScriptingEditor] Configuration applied to block " << _configuredBlock->id << std::endl;
    }
    closeConfigDialog();
}

void ScriptingEditor::drawConfigDialog() {
    if (!_configuredBlock) return;
    
    // Get configuration template for this block type
    BlockConfigTemplate& configTemplate = getConfigTemplate(_configuredBlock->type);
    
    // Draw dark overlay over entire screen
    float screenWidth = GetScreenWidth();
    float screenHeight = GetScreenHeight();
    DrawRectangle(0, 0, screenWidth, screenHeight, Color{0, 0, 0, 150});
    
    // Use template dialog size or default
    float dialogWidth = configTemplate.dialogWidth;
    float dialogHeight = configTemplate.dialogHeight;
    
    // Adjust height based on number of fields
    if (!configTemplate.fields.empty()) {
        float estimatedHeight = 120 + (configTemplate.fields.size() * 60); // Base + field estimate
        dialogHeight = std::max(dialogHeight, estimatedHeight);
    }
    
    // Center the dialog on screen
    Rectangle dialogBounds = {
        (screenWidth - dialogWidth) / 2,
        (screenHeight - dialogHeight) / 2,
        dialogWidth,
        dialogHeight
    };
    
    // Draw dialog background with enhanced styling
    Rectangle shadowRect = {dialogBounds.x + 3, dialogBounds.y + 3, dialogBounds.width, dialogBounds.height};
    DrawRectangleRec(shadowRect, UI::SHADOW_MEDIUM);
    
    DrawRectangleRec(dialogBounds, UI::PANEL_BACKGROUND);
    DrawRectangleLinesEx(dialogBounds, 2, UI::PANEL_BORDER);
    
    // Title bar with accent color
    Rectangle titleBar = {dialogBounds.x, dialogBounds.y, dialogBounds.width, 35};
    DrawRectangleRec(titleBar, UI::ACCENT_PRIMARY);
    
    Rectangle titleArea = {dialogBounds.x + 10, dialogBounds.y + 8, dialogBounds.width - 20, 20};
    std::string title = configTemplate.title.empty() ? 
        ("Configure: " + _configuredBlock->title) : configTemplate.title;
    GuiLabel(titleArea, title.c_str());
    
    // Content area
    float yOffset = 50;
    
    // Draw fields using modular system
    if (configTemplate.fields.empty()) {
        // No configuration fields
        Rectangle noConfigLabel = {dialogBounds.x + 10, dialogBounds.y + yOffset, dialogBounds.width - 20, 20};
        GuiLabel(noConfigLabel, "No configuration options for this block.");
        yOffset += 30;
    } else {
        // Draw each field
        for (const auto& field : configTemplate.fields) {
            auto stateIt = _fieldStates.find(field.key);
            if (stateIt != _fieldStates.end()) {
                yOffset = drawConfigField(field, stateIt->second, dialogBounds, yOffset);
                yOffset += 10; // Spacing between fields
            }
        }
    }
    
    // Buttons at bottom
    float buttonY = dialogBounds.y + dialogBounds.height - 45;
    Rectangle applyButton = {dialogBounds.x + dialogBounds.width - 180, buttonY, 80, 35};
    Rectangle cancelButton = {dialogBounds.x + dialogBounds.width - 90, buttonY, 80, 35};
    
    // Enhanced button styling
    bool applyHovered = CheckCollisionPointRec(GetMousePosition(), applyButton);
    bool cancelHovered = CheckCollisionPointRec(GetMousePosition(), cancelButton);
    
    Color applyColor = applyHovered ? UI::ACCENT_SECONDARY : UI::ACCENT_PRIMARY;
    Color cancelColor = cancelHovered ? UI::UI_TERTIARY : UI::UI_SECONDARY;
    
    DrawRectangleRec(applyButton, applyColor);
    DrawRectangleRec(cancelButton, cancelColor);
    DrawRectangleLinesEx(applyButton, 1, UI::PANEL_BORDER);
    DrawRectangleLinesEx(cancelButton, 1, UI::PANEL_BORDER);
    
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
    
    // Remove all connections involving this block
    removeAllConnectionsForBlock(block->id);
    
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

// ==========================================
// Modern Context Menu System Implementation
// ==========================================

void ScriptingEditor::showContextMenu(ScriptBlock* block, Vector2 mousePosition) {
    if (!block) return;
    
    _contextMenu.clear();
    _contextMenu.isVisible = true;
    _contextMenu.targetBlock = block;
    
    // Create menu items for this block
    createContextMenuItems(block);
    
    // Calculate optimal menu position
    float menuHeight = _contextMenu.getMenuHeight();
    _contextMenu.position = calculateContextMenuPosition(mousePosition, _contextMenu.width, menuHeight);
    
    std::cout << "[ScriptingEditor] Context menu opened for block: " << block->title << std::endl;
}

void ScriptingEditor::hideContextMenu() {
    if (_contextMenu.isVisible) {
        _contextMenu.clear();
        std::cout << "[ScriptingEditor] Context menu closed" << std::endl;
    }
}

void ScriptingEditor::createContextMenuItems(ScriptBlock* block) {
    if (!block) return;
    
    _contextMenu.items.clear();
    
    // Edit item - only enabled if block has configurable parameters
    bool hasParameters = (block->config.floatParams.size() > 0 || 
                         block->config.vectorParams.size() > 0 || 
                         block->config.stringParams.size() > 0 || 
                         block->config.boolParams.size() > 0);
    
    Color editTextColor = hasParameters ? UI::UI_TEXT_PRIMARY : UI::UI_TEXT_TERTIARY;
    Color editHoverColor = hasParameters ? UI::ACCENT_PRIMARY : UI::UI_TERTIARY;
    
    _contextMenu.items.emplace_back("edit", "Edit", editTextColor, editHoverColor, hasParameters);
    
    // Duplicate item - always enabled
    _contextMenu.items.emplace_back("duplicate", "Duplicate", UI::UI_TEXT_PRIMARY, UI::ACCENT_SECONDARY, true);
    
    // Delete item - always enabled, but with danger color
    _contextMenu.items.emplace_back("delete", "Delete", UI::UI_TEXT_PRIMARY, UI::ACCENT_DANGER, true);
}

Vector2 ScriptingEditor::calculateContextMenuPosition(Vector2 requestedPos, float menuWidth, float menuHeight) {
    Vector2 finalPos = requestedPos;
    
    // Get screen bounds
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();
    
    // Add some padding from screen edges
    const float screenPadding = 10.0f;
    
    // Adjust horizontal position if menu would go off-screen
    if (finalPos.x + menuWidth > screenWidth - screenPadding) {
        finalPos.x = screenWidth - menuWidth - screenPadding;
    }
    if (finalPos.x < screenPadding) {
        finalPos.x = screenPadding;
    }
    
    // Adjust vertical position if menu would go off-screen
    if (finalPos.y + menuHeight > screenHeight - screenPadding) {
        finalPos.y = screenHeight - menuHeight - screenPadding;
    }
    if (finalPos.y < screenPadding) {
        finalPos.y = screenPadding;
    }
    
    return finalPos;
}

void ScriptingEditor::drawContextMenu() {
    if (!_contextMenu.isVisible || _contextMenu.items.empty()) {
        return;
    }
    
    const float borderRadius = UI::UI_BORDER_RADIUS_MEDIUM;
    const float shadowOffset = UI::UI_SHADOW_OFFSET_MEDIUM;
    const float padding = UI::UI_PADDING_MEDIUM;
    
    Vector2 menuPos = _contextMenu.position;
    float menuWidth = _contextMenu.width;
    float menuHeight = _contextMenu.getMenuHeight();
    
    Rectangle menuBounds = {menuPos.x, menuPos.y, menuWidth, menuHeight};
    
    // Draw shadow for depth
    Rectangle shadowBounds = {
        menuPos.x + shadowOffset, 
        menuPos.y + shadowOffset, 
        menuWidth, 
        menuHeight
    };
    DrawRectangleRounded(shadowBounds, borderRadius / menuWidth, 8, UI::SHADOW_MEDIUM);
    
    // Draw menu background
    DrawRectangleRounded(menuBounds, borderRadius / menuWidth, 8, UI::PANEL_BACKGROUND);
    DrawRectangleRoundedLines(menuBounds, borderRadius / menuWidth, 8, 1.0f, UI::PANEL_BORDER);
    
    // Draw menu items
    for (size_t i = 0; i < _contextMenu.items.size(); ++i) {
        const ContextMenuItem& item = _contextMenu.items[i];
        
        Rectangle itemBounds = {
            menuPos.x + padding,
            menuPos.y + padding + i * _contextMenu.itemHeight,
            menuWidth - 2 * padding,
            _contextMenu.itemHeight
        };
        
        // Draw hover background if this item is hovered
        if (_contextMenu.hoveredItemIndex == static_cast<int>(i) && item.enabled) {
            DrawRectangleRounded(itemBounds, 
                               (borderRadius * 0.5f) / itemBounds.width, 
                               4, 
                               item.hoverColor);
        }
        
        // Draw item text
        Vector2 textPos = {
            itemBounds.x + UI::UI_PADDING_MEDIUM,
            itemBounds.y + (itemBounds.height - UI::UI_FONT_SIZE_MEDIUM) / 2
        };
        
        DrawTextEx(GetFontDefault(), 
                  item.label.c_str(), 
                  textPos, 
                  UI::UI_FONT_SIZE_MEDIUM, 
                  1.0f, 
                  item.textColor);
    }
}

void ScriptingEditor::updateContextMenu(Vector2 mousePosition) {
    if (!_contextMenu.isVisible) {
        return;
    }
    
    const float padding = UI::UI_PADDING_MEDIUM;
    Vector2 menuPos = _contextMenu.position;
    float menuWidth = _contextMenu.width;
    float menuHeight = _contextMenu.getMenuHeight();
    
    Rectangle menuBounds = {menuPos.x, menuPos.y, menuWidth, menuHeight};
    
    // Update hovered item
    _contextMenu.hoveredItemIndex = -1;
    
    if (CheckCollisionPointRec(mousePosition, menuBounds)) {
        // Calculate which item is hovered
        float relativeY = mousePosition.y - menuPos.y - padding;
        int itemIndex = static_cast<int>(relativeY / _contextMenu.itemHeight);
        
        if (itemIndex >= 0 && itemIndex < static_cast<int>(_contextMenu.items.size())) {
            _contextMenu.hoveredItemIndex = itemIndex;
        }
        
        // Handle click on menu items
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && _contextMenu.hoveredItemIndex >= 0) {
            const ContextMenuItem& clickedItem = _contextMenu.items[_contextMenu.hoveredItemIndex];
            if (clickedItem.enabled) {
                handleContextMenuAction(clickedItem.id);
            }
        }
    } else {
        // Click outside menu - close it
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            hideContextMenu();
        }
    }
}

void ScriptingEditor::handleContextMenuAction(const std::string& actionId) {
    if (!_contextMenu.targetBlock) {
        hideContextMenu();
        return;
    }
    
    ScriptBlock* targetBlock = _contextMenu.targetBlock;
    
    // Execute the action
    if (actionId == "edit") {
        editBlock(targetBlock);
    } else if (actionId == "duplicate") {
        duplicateBlock(targetBlock);
    } else if (actionId == "delete") {
        deleteBlock(targetBlock);
    }
    
    // Close the menu after action
    hideContextMenu();
    
    std::cout << "[ScriptingEditor] Context menu action executed: " << actionId << std::endl;
}

void ScriptingEditor::handleRightClick(Vector2 mousePosition) {
    int selectedObjId = getSelectedObjectId();
    if (selectedObjId == -1) return;
    
    if (isPositionInCanvas(mousePosition)) {
        ScriptBlock* block = getCanvasBlockAtPosition(mousePosition);
        if (block) {
            // Right-clicked on a block - show context menu
            showContextMenu(block, mousePosition);
        } else {
            // Right-clicked on empty canvas - hide any open context menu
            hideContextMenu();
        }
    } else {
        // Right-clicked outside canvas - hide context menu
        hideContextMenu();
    }
}

// ==========================================
// Modern Connection System Implementation
// ==========================================

void ScriptingEditor::updateConnectionSystem(Vector2 mousePosition, bool mousePressed, bool mouseReleased) {
    // Handle connection drag lifecycle
    if (_connectionDrag.isDragging) {
        _connectionDrag.currentMousePosition = mousePosition;
        
        if (mouseReleased) {
            endConnectionDrag(mousePosition);
        } else {
            updateConnectionDrag(mousePosition);
        }
        return;
    }
    
    // Start new connection drag if pressing on a port
    if (mousePressed && isPositionInCanvas(mousePosition)) {
        ScriptBlock* clickedBlock = nullptr;
        int portIndex = -1;
        ConnectionPoint* port = getConnectionPointAt(mousePosition, &clickedBlock, &portIndex);
        
        if (port && clickedBlock) {
            // Only start drag from output ports
            if (port->type == ConnectionPortType::EXECUTION_OUT || 
                port->type == ConnectionPortType::TRUE_OUT || 
                port->type == ConnectionPortType::FALSE_OUT ||
                port->type == ConnectionPortType::VALUE_OUT) {
                startConnectionDrag(clickedBlock, portIndex, port->type, mousePosition);
            }
        }
    }
}

void ScriptingEditor::startConnectionDrag(ScriptBlock* block, int portIndex, ConnectionPortType portType, Vector2 startPos) {
    if (!block) return;
    
    _connectionDrag.isDragging = true;
    _connectionDrag.sourceBlock = block;
    _connectionDrag.sourcePortIndex = portIndex;
    _connectionDrag.sourcePortType = portType;
    _connectionDrag.dragStartPosition = startPos;
    _connectionDrag.currentMousePosition = startPos;
    _connectionDrag.previewColor = getConnectionColor(portType, ConnectionPortType::EXECUTION_IN);
    
    std::cout << "[ScriptingEditor] Started connection drag from block " << block->id << " port " << portIndex << std::endl;
}

void ScriptingEditor::updateConnectionDrag(Vector2 mousePosition) {
    if (!_connectionDrag.isValid()) return;
    
    _connectionDrag.currentMousePosition = mousePosition;
    
    // Update preview color based on potential target
    ScriptBlock* hoverBlock = nullptr;
    int hoverPortIndex = -1;
    ConnectionPoint* hoverPort = getConnectionPointAt(mousePosition, &hoverBlock, &hoverPortIndex);
    
    if (hoverPort && hoverBlock && hoverBlock != _connectionDrag.sourceBlock) {
        // Check if this would be a valid connection
        if (isConnectionValid(_connectionDrag.sourceBlock, _connectionDrag.sourcePortIndex, hoverBlock, hoverPortIndex)) {
            _connectionDrag.previewColor = UI::SUCCESS; // Green for valid
        } else {
            _connectionDrag.previewColor = UI::ERROR; // Red for invalid
        }
    } else {
        _connectionDrag.previewColor = getConnectionColor(_connectionDrag.sourcePortType, ConnectionPortType::EXECUTION_IN);
    }
}

void ScriptingEditor::endConnectionDrag(Vector2 mousePosition) {
    if (!_connectionDrag.isValid()) {
        _connectionDrag.clear();
        return;
    }
    
    // Try to create connection at drop location
    ScriptBlock* targetBlock = nullptr;
    int targetPortIndex = -1;
    ConnectionPoint* targetPort = getConnectionPointAt(mousePosition, &targetBlock, &targetPortIndex);
    
    if (targetPort && targetBlock && targetBlock != _connectionDrag.sourceBlock) {
        if (createConnection(_connectionDrag.sourceBlock, _connectionDrag.sourcePortIndex, targetBlock, targetPortIndex)) {
            std::cout << "[ScriptingEditor] Connection created successfully" << std::endl;
        } else {
            std::cout << "[ScriptingEditor] Connection creation failed - invalid connection" << std::endl;
        }
    }
    
    _connectionDrag.clear();
}

bool ScriptingEditor::createConnection(ScriptBlock* fromBlock, int fromPortIndex, ScriptBlock* toBlock, int toPortIndex) {
    if (!fromBlock || !toBlock || fromBlock == toBlock) return false;
    if (fromPortIndex < 0 || toPortIndex < 0) return false;
    if (fromPortIndex >= fromBlock->outputPorts.size() || toPortIndex >= toBlock->inputPorts.size()) return false;
    
    // Validate connection compatibility
    if (!isConnectionValid(fromBlock, fromPortIndex, toBlock, toPortIndex)) return false;
    
    int selectedObjId = getSelectedObjectId();
    if (selectedObjId == -1) return false;
    
    auto it = _objectScripts.find(selectedObjId);
    if (it == _objectScripts.end()) return false;
    
    VisualScript& script = it->second;
    
    // Remove any existing connections to the target port (each input can only have one connection)
    removeConnection(-1, toBlock->id, -1, toPortIndex);
    
    // Create new connection
    ConnectionPoint& fromPort = fromBlock->outputPorts[fromPortIndex];
    ConnectionPoint& toPort = toBlock->inputPorts[toPortIndex];
    
    Color connectionColor = getConnectionColor(fromPort.type, toPort.type);
    
    BlockConnection newConnection(
        fromBlock->id, toBlock->id,
        fromPort.type, toPort.type,
        fromPortIndex, toPortIndex,
        fromPort.position, toPort.position,
        connectionColor
    );
    
    script.connections.push_back(newConnection);
    
    std::cout << "[ScriptingEditor] Created connection: Block " << fromBlock->id 
              << " port " << fromPortIndex << " -> Block " << toBlock->id 
              << " port " << toPortIndex << std::endl;
    
    return true;
}

void ScriptingEditor::removeConnection(int fromBlockId, int toBlockId, int fromPortIndex, int toPortIndex) {
    int selectedObjId = getSelectedObjectId();
    if (selectedObjId == -1) return;
    
    auto it = _objectScripts.find(selectedObjId);
    if (it == _objectScripts.end()) return;
    
    auto& connections = it->second.connections;
    
    // Remove matching connections
    connections.erase(
        std::remove_if(connections.begin(), connections.end(),
            [fromBlockId, toBlockId, fromPortIndex, toPortIndex](const BlockConnection& conn) {
                bool matchFrom = (fromBlockId == -1 || conn.fromBlockId == fromBlockId);
                bool matchTo = (toBlockId == -1 || conn.toBlockId == toBlockId);
                bool matchFromPort = (fromPortIndex == -1 || conn.fromPortIndex == fromPortIndex);
                bool matchToPort = (toPortIndex == -1 || conn.toPortIndex == toPortIndex);
                return matchFrom && matchTo && matchFromPort && matchToPort;
            }),
        connections.end()
    );
}

void ScriptingEditor::removeAllConnectionsForBlock(int blockId) {
    removeConnection(blockId, -1); // Remove as source
    removeConnection(-1, blockId); // Remove as target
}

void ScriptingEditor::drawConnections(const VisualScript& script) {
    // Update connection positions before drawing
    for (const auto& connection : script.connections) {
        // Find the blocks
        const ScriptBlock* fromBlock = nullptr;
        const ScriptBlock* toBlock = nullptr;
        
        for (const auto& block : script.blocks) {
            if (block.id == connection.fromBlockId) fromBlock = &block;
            if (block.id == connection.toBlockId) toBlock = &block;
        }
        
        if (fromBlock && toBlock && connection.fromPortIndex < fromBlock->outputPorts.size() 
            && connection.toPortIndex < toBlock->inputPorts.size()) {
            
            Vector2 startPoint = fromBlock->outputPorts[connection.fromPortIndex].position;
            Vector2 endPoint = toBlock->inputPorts[connection.toPortIndex].position;
            
            // Apply canvas offset
            startPoint.x += _canvasOffset.x;
            startPoint.y += _canvasOffset.y;
            endPoint.x += _canvasOffset.x;
            endPoint.y += _canvasOffset.y;
            
            drawConnectionLine(startPoint, endPoint, connection.connectionColor, 3.0f);
        }
    }
}

void ScriptingEditor::drawConnectionLine(Vector2 start, Vector2 end, Color color, float thickness) {
    // Calculate control points for smooth bezier curve
    float distance = abs(end.x - start.x);
    float curvature = std::min(distance * 0.5f, 100.0f); // Adaptive curvature
    
    Vector2 control1 = {start.x + curvature, start.y};
    Vector2 control2 = {end.x - curvature, end.y};
    
    // Draw bezier curve with multiple segments for smoothness
    const int segments = 20;
    Vector2 lastPoint = start;
    
    for (int i = 1; i <= segments; i++) {
        float t = (float)i / segments;
        float u = 1.0f - t;
        float tt = t * t;
        float uu = u * u;
        float uuu = uu * u;
        float ttt = tt * t;
        
        Vector2 point = {
            uuu * start.x + 3 * uu * t * control1.x + 3 * u * tt * control2.x + ttt * end.x,
            uuu * start.y + 3 * uu * t * control1.y + 3 * u * tt * control2.y + ttt * end.y
        };
        
        DrawLineEx(lastPoint, point, thickness, color);
        lastPoint = point;
    }
    
    // Draw arrow at the end
    Vector2 direction = {end.x - control2.x, end.y - control2.y};
    float length = sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0.1f) {
        direction.x /= length;
        direction.y /= length;
        
        Vector2 arrowLeft = {end.x - direction.x * 10 - direction.y * 5, 
                            end.y - direction.y * 10 + direction.x * 5};
        Vector2 arrowRight = {end.x - direction.x * 10 + direction.y * 5, 
                             end.y - direction.y * 10 - direction.x * 5};
        
        DrawLineEx(end, arrowLeft, thickness, color);
        DrawLineEx(end, arrowRight, thickness, color);
    }
}

void ScriptingEditor::drawConnectionPreview() {
    if (!_connectionDrag.isValid()) return;
    
    Vector2 start = _connectionDrag.dragStartPosition;
    Vector2 end = _connectionDrag.currentMousePosition;
    
    // Apply canvas offset
    start.x += _canvasOffset.x;
    start.y += _canvasOffset.y;
    end.x += _canvasOffset.x;
    end.y += _canvasOffset.y;
    
    // Draw preview with slightly transparent color
    Color previewColor = _connectionDrag.previewColor;
    previewColor.a = 180; // Semi-transparent
    
    drawConnectionLine(start, end, previewColor, 2.5f);
}

bool ScriptingEditor::isConnectionValid(ScriptBlock* fromBlock, int fromPortIndex, ScriptBlock* toBlock, int toPortIndex) {
    if (!fromBlock || !toBlock || fromBlock == toBlock) return false;
    if (fromPortIndex < 0 || toPortIndex < 0) return false;
    if (fromPortIndex >= fromBlock->outputPorts.size() || toPortIndex >= toBlock->inputPorts.size()) return false;
    
    ConnectionPortType fromType = fromBlock->outputPorts[fromPortIndex].type;
    ConnectionPortType toType = toBlock->inputPorts[toPortIndex].type;
    
    // Execution flow connections
    if (fromType == ConnectionPortType::EXECUTION_OUT && toType == ConnectionPortType::EXECUTION_IN) return true;
    if (fromType == ConnectionPortType::TRUE_OUT && toType == ConnectionPortType::EXECUTION_IN) return true;
    if (fromType == ConnectionPortType::FALSE_OUT && toType == ConnectionPortType::EXECUTION_IN) return true;
    
    // Value connections (for future data flow)
    if (fromType == ConnectionPortType::VALUE_OUT && toType == ConnectionPortType::VALUE_IN) return true;
    
    return false;
}

ConnectionPoint* ScriptingEditor::getConnectionPointAt(Vector2 position, ScriptBlock** outBlock, int* outPortIndex) {
    int selectedObjId = getSelectedObjectId();
    if (selectedObjId == -1) return nullptr;
    
    auto it = _objectScripts.find(selectedObjId);
    if (it == _objectScripts.end()) return nullptr;
    
    const float portRadius = 8.0f; // Port hit detection radius
    
    for (auto& block : it->second.blocks) {
        if (!block.isOnCanvas) continue;
        
        // Check input ports
        for (size_t i = 0; i < block.inputPorts.size(); ++i) {
            Vector2 portPos = block.inputPorts[i].position;
            float distance = sqrt(pow(position.x - portPos.x, 2) + pow(position.y - portPos.y, 2));
            
            if (distance <= portRadius) {
                if (outBlock) *outBlock = &block;
                if (outPortIndex) *outPortIndex = static_cast<int>(i);
                return &block.inputPorts[i];
            }
        }
        
        // Check output ports
        for (size_t i = 0; i < block.outputPorts.size(); ++i) {
            Vector2 portPos = block.outputPorts[i].position;
            float distance = sqrt(pow(position.x - portPos.x, 2) + pow(position.y - portPos.y, 2));
            
            if (distance <= portRadius) {
                if (outBlock) *outBlock = &block;
                if (outPortIndex) *outPortIndex = static_cast<int>(i);
                return &block.outputPorts[i];
            }
        }
    }
    
    return nullptr;
}

Color ScriptingEditor::getConnectionColor(ConnectionPortType fromType, ConnectionPortType toType) {
    return BLACK;
}

void ScriptingEditor::updateConnectionPositions(VisualScript& script) {
    // Update cached positions in connections
    for (auto& connection : script.connections) {
        // Find the blocks
        ScriptBlock* fromBlock = nullptr;
        ScriptBlock* toBlock = nullptr;
        
        for (auto& block : script.blocks) {
            if (block.id == connection.fromBlockId) fromBlock = &block;
            if (block.id == connection.toBlockId) toBlock = &block;
        }
        
        if (fromBlock && toBlock && connection.fromPortIndex < fromBlock->outputPorts.size() 
            && connection.toPortIndex < toBlock->inputPorts.size()) {
            
            connection.fromPoint = fromBlock->outputPorts[connection.fromPortIndex].position;
            connection.toPoint = toBlock->inputPorts[connection.toPortIndex].position;
        }
    }
}

// ==========================================
// Script Compilation System Implementation
// ==========================================

CompiledScript VisualScript::compileToExecutionFlow() const {
    CompiledScript compiled(objectId, name);
    
    // First, validate all connections
    std::string validationErrors;
    if (!validateConnections(validationErrors)) {
        compiled.errors = validationErrors;
        return compiled;
    }
    
    // Create nodes for each block
    for (const auto& block : blocks) {
        if (!block.isOnCanvas) continue;
        
        CompiledScriptNode node(block.id, block.type, block.config);
        
        // Mark entry points (blocks with no execution input)
        bool hasExecutionInput = false;
        for (const auto& connection : connections) {
            if (connection.toBlockId == block.id && 
                (connection.toPortType == ConnectionPortType::EXECUTION_IN)) {
                hasExecutionInput = true;
                break;
            }
        }
        
        // Event blocks are always entry points
        if (block.type == BlockType::ON_START || block.type == BlockType::ON_CLICK || 
            block.type == BlockType::ON_UPDATE || block.type == BlockType::ON_KEY_PRESS) {
            node.isEntryPoint = true;
            compiled.entryPoints.push_back(block.id);
        } else if (!hasExecutionInput) {
            node.isEntryPoint = true;
            compiled.entryPoints.push_back(block.id);
        }
        
        // Find connected next nodes
        for (const auto& connection : connections) {
            if (connection.fromBlockId == block.id) {
                if (connection.fromPortType == ConnectionPortType::EXECUTION_OUT) {
                    node.nextNodes.push_back(connection.toBlockId);
                } else if (connection.fromPortType == ConnectionPortType::TRUE_OUT) {
                    node.trueNextNode = connection.toBlockId;
                } else if (connection.fromPortType == ConnectionPortType::FALSE_OUT) {
                    node.falseNextNode = connection.toBlockId;
                }
            }
        }
        
        compiled.nodes.push_back(node);
    }
    
    compiled.isValid = true;
    return compiled;
}

bool VisualScript::validateConnections(std::string& errors) const {
    errors.clear();
    bool isValid = true;
    
    // Check for orphaned blocks (except entry points)
    for (const auto& block : blocks) {
        if (!block.isOnCanvas) continue;
        
        bool isEntryPoint = (block.type == BlockType::ON_START || block.type == BlockType::ON_CLICK || 
                           block.type == BlockType::ON_UPDATE || block.type == BlockType::ON_KEY_PRESS);
        
        if (!isEntryPoint) {
            bool hasInput = false;
            for (const auto& connection : connections) {
                if (connection.toBlockId == block.id && 
                    connection.toPortType == ConnectionPortType::EXECUTION_IN) {
                    hasInput = true;
                    break;
                }
            }
            
            if (!hasInput) {
                errors += "Block " + std::to_string(block.id) + " (" + block.title + ") has no input connection.\n";
                isValid = false;
            }
        }
    }
    
    // Check for invalid connections
    for (const auto& connection : connections) {
        // Find source and target blocks
        const ScriptBlock* fromBlock = nullptr;
        const ScriptBlock* toBlock = nullptr;
        
        for (const auto& block : blocks) {
            if (block.id == connection.fromBlockId) fromBlock = &block;
            if (block.id == connection.toBlockId) toBlock = &block;
        }
        
        if (!fromBlock || !toBlock) {
            errors += "Connection references non-existent block(s).\n";
            isValid = false;
            continue;
        }
        
        // Validate port indices
        if (connection.fromPortIndex >= fromBlock->outputPorts.size() ||
            connection.toPortIndex >= toBlock->inputPorts.size()) {
            errors += "Connection has invalid port indices.\n";
            isValid = false;
        }
    }
    
    return isValid;
}

CompiledScript ScriptingEditor::compileScript(int objectId) {
    auto it = _objectScripts.find(objectId);
    if (it == _objectScripts.end()) {
        CompiledScript empty(objectId);
        empty.errors = "No script found for object " + std::to_string(objectId);
        return empty;
    }
    
    CompiledScript compiled = it->second.compileToExecutionFlow();
    
    if (compiled.isValid) {
        std::cout << "[ScriptingEditor] Successfully compiled script for object " << objectId 
                  << " with " << compiled.nodes.size() << " nodes and " 
                  << compiled.entryPoints.size() << " entry points" << std::endl;
    } else {
        std::cout << "[ScriptingEditor] Compilation failed for object " << objectId 
                  << ": " << compiled.errors << std::endl;
    }
    
    return compiled;
}

bool ScriptingEditor::validateScript(int objectId, std::string& errors) {
    auto it = _objectScripts.find(objectId);
    if (it == _objectScripts.end()) {
        errors = "No script found for object " + std::to_string(objectId);
        return false;
    }
    
    return it->second.validateConnections(errors);
}

void ScriptingEditor::exportCompiledScripts(const std::string& filePath) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cout << "[ScriptingEditor] Failed to open file for export: " << filePath << std::endl;
        return;
    }
    
    file << "{\n";
    file << "  \"compiledScripts\": [\n";
    
    bool first = true;
    for (const auto& pair : _objectScripts) {
        if (!first) file << ",\n";
        first = false;
        
        CompiledScript compiled = compileScript(pair.first);
        
        file << "    {\n";
        file << "      \"objectId\": " << compiled.objectId << ",\n";
        file << "      \"name\": \"" << compiled.name << "\",\n";
        file << "      \"isValid\": " << (compiled.isValid ? "true" : "false") << ",\n";
        
        if (!compiled.isValid) {
            file << "      \"errors\": \"" << compiled.errors << "\",\n";
        }
        
        file << "      \"entryPoints\": [";
        for (size_t i = 0; i < compiled.entryPoints.size(); ++i) {
            if (i > 0) file << ", ";
            file << compiled.entryPoints[i];
        }
        file << "],\n";
        
        file << "      \"nodes\": [\n";
        for (size_t i = 0; i < compiled.nodes.size(); ++i) {
            if (i > 0) file << ",\n";
            const auto& node = compiled.nodes[i];
            
            file << "        {\n";
            file << "          \"blockId\": " << node.blockId << ",\n";
            file << "          \"blockType\": " << static_cast<int>(node.blockType) << ",\n";
            file << "          \"isEntryPoint\": " << (node.isEntryPoint ? "true" : "false") << ",\n";
            
            file << "          \"nextNodes\": [";
            for (size_t j = 0; j < node.nextNodes.size(); ++j) {
                if (j > 0) file << ", ";
                file << node.nextNodes[j];
            }
            file << "],\n";
            
            file << "          \"trueNextNode\": " << node.trueNextNode << ",\n";
            file << "          \"falseNextNode\": " << node.falseNextNode << "\n";
            file << "        }";
        }
        file << "\n      ]\n";
        file << "    }";
    }
    
    file << "\n  ]\n";
    file << "}\n";
    
    file.close();
    std::cout << "[ScriptingEditor] Exported " << _objectScripts.size() 
              << " compiled scripts to " << filePath << std::endl;
}

void ScriptingEditor::saveCurrentScript() {
    int selectedObjId = getSelectedObjectId();
    if (selectedObjId == -1) {
        std::cout << "[ScriptingEditor] No object selected for saving" << std::endl;
        return;
    }
    
    auto it = _objectScripts.find(selectedObjId);
    if (it == _objectScripts.end()) {
        std::cout << "[ScriptingEditor] No script found for object " << selectedObjId << std::endl;
        return;
    }
    
    // Ensure scripts directory exists
    std::string scriptsDir = getScriptsDirectory();
    if (!createDirectoryIfNotExists(scriptsDir)) {
        std::cout << "[ScriptingEditor] Failed to create scripts directory: " << scriptsDir << std::endl;
        return;
    }
    
    std::string filename = scriptsDir + "/script_object_" + std::to_string(selectedObjId) + ".json";
    saveScriptToFile(it->second, filename);
}

void ScriptingEditor::saveScriptToFile(const VisualScript& script, const std::string& filepath) {
    std::string jsonContent = scriptToJson(script);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cout << "[ScriptingEditor] Failed to create file: " << filepath << std::endl;
        return;
    }
    
    file << jsonContent;
    file.close();
    
    std::cout << "[ScriptingEditor] Script saved to: " << filepath << std::endl;
}

void ScriptingEditor::loadScriptFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cout << "[ScriptingEditor] Failed to open file: " << filepath << std::endl;
        return;
    }
    
    std::string jsonContent((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    VisualScript loadedScript = jsonToScript(jsonContent);
    if (loadedScript.objectId != -1) {
        _objectScripts[loadedScript.objectId] = loadedScript;
        std::cout << "[ScriptingEditor] Script loaded for object " << loadedScript.objectId << std::endl;
    }
}

void ScriptingEditor::clearAllScripts() {
    _objectScripts.clear();
    _nextBlockId = 1;
    _nextCanvasOrder = 0;
    std::cout << "[ScriptingEditor] All scripts cleared" << std::endl;
}

std::string ScriptingEditor::scriptToJson(const VisualScript& script) {
    std::ostringstream json;
    
    json << "{\n";
    json << "  \"objectId\": " << script.objectId << ",\n";
    json << "  \"name\": \"" << script.name << "\",\n";
    json << "  \"enabled\": " << (script.enabled ? "true" : "false") << ",\n";
    
    // Serialize blocks
    json << "  \"blocks\": [\n";
    for (size_t i = 0; i < script.blocks.size(); ++i) {
        const ScriptBlock& block = script.blocks[i];
        
        json << "    {\n";
        json << "      \"id\": " << block.id << ",\n";
        json << "      \"type\": " << static_cast<int>(block.type) << ",\n";
        json << "      \"position\": {\n";
        json << "        \"x\": " << block.position.x << ",\n";
        json << "        \"y\": " << block.position.y << "\n";
        json << "      },\n";
        json << "      \"size\": {\n";
        json << "        \"width\": " << block.size.x << ",\n";
        json << "        \"height\": " << block.size.y << "\n";
        json << "      },\n";
        json << "      \"title\": \"" << block.title << "\",\n";
        json << "      \"subtitle\": \"" << block.subtitle << "\",\n";
        json << "      \"isOnCanvas\": " << (block.isOnCanvas ? "true" : "false") << ",\n";
        json << "      \"canvasOrder\": " << block.canvasOrder << ",\n";
        
        // Serialize block configuration
        json << "      \"config\": {\n";
        json << "        \"floatParams\": {\n";
        bool firstFloat = true;
        for (const auto& param : block.config.floatParams) {
            if (!firstFloat) json << ",\n";
            json << "          \"" << param.first << "\": " << param.second;
            firstFloat = false;
        }
        json << "\n        },\n";
        
        json << "        \"vectorParams\": {\n";
        bool firstVector = true;
        for (const auto& param : block.config.vectorParams) {
            if (!firstVector) json << ",\n";
            json << "          \"" << param.first << "\": {\n";
            json << "            \"x\": " << param.second.x << ",\n";
            json << "            \"y\": " << param.second.y << ",\n";
            json << "            \"z\": " << param.second.z << "\n";
            json << "          }";
            firstVector = false;
        }
        json << "\n        },\n";
        
        json << "        \"stringParams\": {\n";
        bool firstString = true;
        for (const auto& param : block.config.stringParams) {
            if (!firstString) json << ",\n";
            json << "          \"" << param.first << "\": \"" << param.second << "\"";
            firstString = false;
        }
        json << "\n        },\n";
        
        json << "        \"boolParams\": {\n";
        bool firstBool = true;
        for (const auto& param : block.config.boolParams) {
            if (!firstBool) json << ",\n";
            json << "          \"" << param.first << "\": " << (param.second ? "true" : "false");
            firstBool = false;
        }
        json << "\n        }\n";
        json << "      }\n";
        json << "    }";
        
        if (i < script.blocks.size() - 1) json << ",";
        json << "\n";
    }
    json << "  ],\n";
    
    // Serialize connections
    json << "  \"connections\": [\n";
    for (size_t i = 0; i < script.connections.size(); ++i) {
        const BlockConnection& conn = script.connections[i];
        
        json << "    {\n";
        json << "      \"fromBlockId\": " << conn.fromBlockId << ",\n";
        json << "      \"toBlockId\": " << conn.toBlockId << ",\n";
        json << "      \"fromPortType\": " << static_cast<int>(conn.fromPortType) << ",\n";
        json << "      \"toPortType\": " << static_cast<int>(conn.toPortType) << ",\n";
        json << "      \"fromPortIndex\": " << conn.fromPortIndex << ",\n";
        json << "      \"toPortIndex\": " << conn.toPortIndex << ",\n";
        json << "      \"isValid\": " << (conn.isValid ? "true" : "false") << "\n";
        json << "    }";
        
        if (i < script.connections.size() - 1) json << ",";
        json << "\n";
    }
    json << "  ]\n";
    json << "}";
    
    return json.str();
}

VisualScript ScriptingEditor::jsonToScript(const std::string& jsonContent) {
    VisualScript script;
    
    // Simple JSON parsing (basic implementation)
    // In a production system, you would use a proper JSON library like nlohmann/json
    
    // Extract objectId
    size_t objectIdPos = jsonContent.find("\"objectId\":");
    if (objectIdPos != std::string::npos) {
        size_t numStart = jsonContent.find_first_of("0123456789-", objectIdPos);
        size_t numEnd = jsonContent.find_first_of(",\n}", numStart);
        if (numStart != std::string::npos && numEnd != std::string::npos) {
            script.objectId = std::stoi(jsonContent.substr(numStart, numEnd - numStart));
        }
    }
    
    // Extract name
    size_t namePos = jsonContent.find("\"name\":");
    if (namePos != std::string::npos) {
        size_t nameStart = jsonContent.find("\"", namePos + 7) + 1;
        size_t nameEnd = jsonContent.find("\"", nameStart);
        if (nameStart != std::string::npos && nameEnd != std::string::npos) {
            script.name = jsonContent.substr(nameStart, nameEnd - nameStart);
        }
    }
    
    // Extract enabled
    size_t enabledPos = jsonContent.find("\"enabled\":");
    if (enabledPos != std::string::npos) {
        script.enabled = jsonContent.find("true", enabledPos) < jsonContent.find("false", enabledPos);
    }
    
    std::cout << "[ScriptingEditor] Basic JSON parsing implemented. For full loading functionality, " 
              << "integrate a proper JSON library like nlohmann/json" << std::endl;
    
    return script;
}

bool ScriptingEditor::createDirectoryIfNotExists(const std::string& path) {
    struct stat info;
    
    // Check if directory already exists
    if (stat(path.c_str(), &info) == 0 && (info.st_mode & S_IFDIR)) {
        return true; // Directory exists
    }
    
    // Try to create the directory
    #ifdef _WIN32
        if (mkdir(path.c_str()) == 0) {
            return true;
        }
    #else
        if (mkdir(path.c_str(), 0755) == 0) {
            return true;
        }
    #endif
    
    // If mkdir failed, try to create parent directories recursively
    size_t pos = path.find_last_of("/\\");
    if (pos != std::string::npos) {
        std::string parentDir = path.substr(0, pos);
        if (createDirectoryIfNotExists(parentDir)) {
            // Try to create the directory again after creating parent
            #ifdef _WIN32
                return mkdir(path.c_str()) == 0;
            #else
                return mkdir(path.c_str(), 0755) == 0;
            #endif
        }
    }
    
    return false;
}

std::string ScriptingEditor::getScriptsDirectory() {
    return "game_project/assets/scripts";
}

// FieldState implementation
void FieldState::setFromString(const std::string& value) {
    stringValue = value;
    strncpy(textBuffer, value.c_str(), sizeof(textBuffer) - 1);
    textBuffer[sizeof(textBuffer) - 1] = '\0';
    isDirty = true;
}

void FieldState::setFromFloat(float value) {
    floatValue = value;
    stringValue = std::to_string(value);
    strncpy(textBuffer, stringValue.c_str(), sizeof(textBuffer) - 1);
    textBuffer[sizeof(textBuffer) - 1] = '\0';
    isDirty = true;
}

void FieldState::setFromBool(bool value) {
    boolValue = value;
    isDirty = true;
}

void FieldState::setFromVector3(const Vector3& value) {
    vectorValue = value;
    isDirty = true;
}

void FieldState::setFromInt(int value) {
    intValue = value;
    floatValue = static_cast<float>(value);
    stringValue = std::to_string(value);
    strncpy(textBuffer, stringValue.c_str(), sizeof(textBuffer) - 1);
    textBuffer[sizeof(textBuffer) - 1] = '\0';
    isDirty = true;
}

// Modular configuration system implementation
void ScriptingEditor::initializeBlockConfigTemplates() {
    // ON_KEY_PRESS block configuration
    _blockConfigTemplates[BlockType::ON_KEY_PRESS] = BlockConfigTemplate("Key Press Event")
        .addField(FieldDefinition("key", "Key", FieldType::COMBO, "Space")
            .withOptions({"Space", "A", "W", "S", "D", "Up", "Down", "Left", "Right", "Enter"})
            .withDescription("The key that triggers this event"));
    
    // MOVE block configuration
    _blockConfigTemplates[BlockType::MOVE] = BlockConfigTemplate("Move Action")
        .addField(FieldDefinition("speed", "Speed", FieldType::FLOAT, "1.0")
            .withRange(0.1f, 50.0f)
            .withDescription("Movement speed in units per second"))
        .addField(FieldDefinition("direction", "Direction", FieldType::VECTOR3, "1.0,0.0,0.0")
            .withDescription("Direction vector (X, Y, Z)"))
        .withSize(450, 400);
    
    // ROTATE block configuration
    _blockConfigTemplates[BlockType::ROTATE] = BlockConfigTemplate("Rotate Action")
        .addField(FieldDefinition("speed", "Speed (deg/sec)", FieldType::FLOAT, "90.0")
            .withRange(0.1f, 360.0f)
            .withDescription("Rotation speed in degrees per second"));
    
    // DELAY block configuration
    _blockConfigTemplates[BlockType::DELAY] = BlockConfigTemplate("Delay Action")
        .addField(FieldDefinition("duration", "Duration (seconds)", FieldType::FLOAT, "1.0")
            .withRange(0.1f, 60.0f)
            .withDescription("How long to wait in seconds"));
    
    // LOG block configuration
    _blockConfigTemplates[BlockType::LOG] = BlockConfigTemplate("Log Action")
        .addField(FieldDefinition("message", "Message", FieldType::STRING, "Debug message")
            .withDescription("Text to display in the console"))
        .withSize(450, 350);
    
    // LOOP block configuration
    _blockConfigTemplates[BlockType::LOOP] = BlockConfigTemplate("Loop Action")
        .addField(FieldDefinition("iterations", "Iterations", FieldType::INTEGER, "5")
            .withRange(1, 1000)
            .withDescription("Number of times to repeat"));
    
    // CHANGE_COLOR block configuration
    _blockConfigTemplates[BlockType::CHANGE_COLOR] = BlockConfigTemplate("Change Color Action")
        .addField(FieldDefinition("color", "Color", FieldType::VECTOR3, "1.0,0.0,0.0")
            .withDescription("RGB color values (0.0 to 1.0)"))
        .addField(FieldDefinition("duration", "Duration", FieldType::FLOAT, "0.5")
            .withRange(0.1f, 10.0f)
            .withDescription("Color transition duration"));
    
    // HIDE/SHOW blocks
    _blockConfigTemplates[BlockType::HIDE] = BlockConfigTemplate("Hide Action");
    _blockConfigTemplates[BlockType::SHOW] = BlockConfigTemplate("Show Action");
    
    // Value blocks
    _blockConfigTemplates[BlockType::VALUE] = BlockConfigTemplate("Value")
        .addField(FieldDefinition("value", "Value", FieldType::FLOAT, "0.0")
            .withDescription("Numeric value"));
    
    _blockConfigTemplates[BlockType::TRUE] = BlockConfigTemplate("True Value");
    _blockConfigTemplates[BlockType::FALSE] = BlockConfigTemplate("False Value");
    
    std::cout << "[ScriptingEditor] Initialized " << _blockConfigTemplates.size() << " block configuration templates" << std::endl;
}

BlockConfigTemplate& ScriptingEditor::getConfigTemplate(BlockType blockType) {
    auto it = _blockConfigTemplates.find(blockType);
    if (it != _blockConfigTemplates.end()) {
        return it->second;
    }
    
    // Return a default empty template for blocks without configuration
    static BlockConfigTemplate defaultTemplate("No Configuration");
    return defaultTemplate;
}

void ScriptingEditor::loadFieldStatesFromBlock(ScriptBlock* block) {
    if (!block) return;
    
    resetFieldStates();
    
    BlockConfigTemplate& configTemplate = getConfigTemplate(block->type);
    
    for (const auto& field : configTemplate.fields) {
        FieldState& state = _fieldStates[field.key];
        
        switch (field.type) {
            case FieldType::FLOAT: {
                auto it = block->config.floatParams.find(field.key);
                if (it != block->config.floatParams.end()) {
                    state.setFromFloat(it->second);
                } else {
                    state.setFromFloat(std::stof(field.defaultValue));
                }
                break;
            }
            case FieldType::STRING:
            case FieldType::COMBO: {
                auto it = block->config.stringParams.find(field.key);
                if (it != block->config.stringParams.end()) {
                    state.setFromString(it->second);
                } else {
                    state.setFromString(field.defaultValue);
                }
                
                // Set combo index for combo fields
                if (field.type == FieldType::COMBO) {
                    auto optIt = std::find(field.options.begin(), field.options.end(), state.stringValue);
                    state.comboIndex = (optIt != field.options.end()) ? 
                        std::distance(field.options.begin(), optIt) : 0;
                }
                break;
            }
            case FieldType::BOOL: {
                auto it = block->config.boolParams.find(field.key);
                if (it != block->config.boolParams.end()) {
                    state.setFromBool(it->second);
                } else {
                    state.setFromBool(field.defaultValue == "true");
                }
                break;
            }
            case FieldType::VECTOR3: {
                auto it = block->config.vectorParams.find(field.key);
                if (it != block->config.vectorParams.end()) {
                    state.setFromVector3(it->second);
                } else {
                    // Parse default value "x,y,z"
                    Vector3 defaultVec = {1.0f, 0.0f, 0.0f};
                    if (!field.defaultValue.empty()) {
                        std::istringstream ss(field.defaultValue);
                        std::string token;
                        int component = 0;
                        while (std::getline(ss, token, ',') && component < 3) {
                            float value = std::stof(token);
                            if (component == 0) defaultVec.x = value;
                            else if (component == 1) defaultVec.y = value;
                            else if (component == 2) defaultVec.z = value;
                            component++;
                        }
                    }
                    state.setFromVector3(defaultVec);
                }
                break;
            }
            case FieldType::INTEGER: {
                auto it = block->config.floatParams.find(field.key);
                if (it != block->config.floatParams.end()) {
                    state.setFromInt(static_cast<int>(it->second));
                } else {
                    state.setFromInt(std::stoi(field.defaultValue));
                }
                break;
            }
        }
    }
}

void ScriptingEditor::applyFieldStatesToBlock(ScriptBlock* block) {
    if (!block) return;
    
    BlockConfigTemplate& configTemplate = getConfigTemplate(block->type);
    
    for (const auto& field : configTemplate.fields) {
        auto stateIt = _fieldStates.find(field.key);
        if (stateIt == _fieldStates.end()) continue;
        
        const FieldState& state = stateIt->second;
        
        switch (field.type) {
            case FieldType::FLOAT:
                block->config.floatParams[field.key] = state.floatValue;
                break;
            case FieldType::STRING:
                block->config.stringParams[field.key] = state.stringValue;
                break;
            case FieldType::COMBO:
                if (state.comboIndex >= 0 && state.comboIndex < field.options.size()) {
                    block->config.stringParams[field.key] = field.options[state.comboIndex];
                }
                break;
            case FieldType::BOOL:
                block->config.boolParams[field.key] = state.boolValue;
                break;
            case FieldType::VECTOR3:
                block->config.vectorParams[field.key] = state.vectorValue;
                break;
            case FieldType::INTEGER:
                block->config.floatParams[field.key] = static_cast<float>(state.intValue);
                break;
        }
    }
    
    // Update block subtitle with parameter summary
    block->subtitle = block->getParameterSummary();
}

void ScriptingEditor::resetFieldStates() {
    _fieldStates.clear();
}

// UI field rendering methods
float ScriptingEditor::drawConfigField(const FieldDefinition& field, FieldState& state, Rectangle dialogBounds, float yOffset) {
    switch (field.type) {
        case FieldType::FLOAT:
            return drawFloatField(field, state, dialogBounds, yOffset);
        case FieldType::STRING:
            return drawStringField(field, state, dialogBounds, yOffset);
        case FieldType::BOOL:
            return drawBoolField(field, state, dialogBounds, yOffset);
        case FieldType::VECTOR3:
            return drawVector3Field(field, state, dialogBounds, yOffset);
        case FieldType::COMBO:
            return drawComboField(field, state, dialogBounds, yOffset);
        case FieldType::INTEGER:
            return drawIntegerField(field, state, dialogBounds, yOffset);
        default:
            return yOffset;
    }
}

float ScriptingEditor::drawFloatField(const FieldDefinition& field, FieldState& state, Rectangle dialogBounds, float yOffset) {
    const float fieldHeight = 50.0f;
    const float padding = 10.0f;
    
    // Draw label
    Rectangle labelRect = {dialogBounds.x + padding, dialogBounds.y + yOffset, dialogBounds.width - 2*padding, 20};
    GuiLabel(labelRect, field.label.c_str());
    
    // Draw input field
    Rectangle inputRect = {dialogBounds.x + padding, dialogBounds.y + yOffset + 25, dialogBounds.width - 2*padding, 25};
    
    // Update text buffer if value changed externally
    if (std::fabs(state.floatValue - std::stof(state.stringValue)) > 0.001f) {
        state.setFromFloat(state.floatValue);
    }
    
    if (GuiTextBox(inputRect, state.textBuffer, sizeof(state.textBuffer), true)) {
        try {
            state.floatValue = std::stof(state.textBuffer);
            state.stringValue = state.textBuffer;
            
            // Clamp to range if specified
            if (field.minValue != field.maxValue) {
                state.floatValue = std::max(field.minValue, std::min(field.maxValue, state.floatValue));
                state.setFromFloat(state.floatValue);
            }
            
            state.isDirty = true;
        } catch (const std::exception&) {
            // Invalid input, revert to previous value
            state.setFromFloat(state.floatValue);
        }
    }
    
    // Show range info if specified
    if (field.minValue != field.maxValue) {
        Rectangle rangeRect = {dialogBounds.x + padding, dialogBounds.y + yOffset + 52, dialogBounds.width - 2*padding, 15};
        std::string rangeText = "Range: " + std::to_string(field.minValue) + " - " + std::to_string(field.maxValue);
        GuiLabel(rangeRect, rangeText.c_str());
        return yOffset + fieldHeight + 20;
    }
    
    return yOffset + fieldHeight;
}

float ScriptingEditor::drawStringField(const FieldDefinition& field, FieldState& state, Rectangle dialogBounds, float yOffset) {
    const float fieldHeight = 50.0f;
    const float padding = 10.0f;
    
    // Draw label
    Rectangle labelRect = {dialogBounds.x + padding, dialogBounds.y + yOffset, dialogBounds.width - 2*padding, 20};
    GuiLabel(labelRect, field.label.c_str());
    
    // Draw input field
    Rectangle inputRect = {dialogBounds.x + padding, dialogBounds.y + yOffset + 25, dialogBounds.width - 2*padding, 25};
    
    if (GuiTextBox(inputRect, state.textBuffer, sizeof(state.textBuffer), true)) {
        state.stringValue = state.textBuffer;
        state.isDirty = true;
    }
    
    return yOffset + fieldHeight;
}

float ScriptingEditor::drawBoolField(const FieldDefinition& field, FieldState& state, Rectangle dialogBounds, float yOffset) {
    const float fieldHeight = 35.0f;
    const float padding = 10.0f;
    
    // Draw checkbox with label
    Rectangle checkRect = {dialogBounds.x + padding, dialogBounds.y + yOffset + 5, 20, 20};
    Rectangle labelRect = {dialogBounds.x + padding + 30, dialogBounds.y + yOffset + 8, dialogBounds.width - 2*padding - 30, 20};
    
    if (GuiCheckBox(checkRect, "", &state.boolValue)) {
        state.isDirty = true;
    }
    
    GuiLabel(labelRect, field.label.c_str());
    
    return yOffset + fieldHeight;
}

float ScriptingEditor::drawVector3Field(const FieldDefinition& field, FieldState& state, Rectangle dialogBounds, float yOffset) {
    const float fieldHeight = 75.0f;
    const float padding = 10.0f;
    const float componentWidth = (dialogBounds.width - 4*padding) / 3;
    
    // Draw label
    Rectangle labelRect = {dialogBounds.x + padding, dialogBounds.y + yOffset, dialogBounds.width - 2*padding, 20};
    GuiLabel(labelRect, field.label.c_str());
    
    // Draw X, Y, Z input fields
    static char xBuffer[32], yBuffer[32], zBuffer[32];
    snprintf(xBuffer, sizeof(xBuffer), "%.3f", state.vectorValue.x);
    snprintf(yBuffer, sizeof(yBuffer), "%.3f", state.vectorValue.y);
    snprintf(zBuffer, sizeof(zBuffer), "%.3f", state.vectorValue.z);
    
    Rectangle xRect = {dialogBounds.x + padding, dialogBounds.y + yOffset + 25, componentWidth, 25};
    Rectangle yRect = {dialogBounds.x + padding + componentWidth + padding, dialogBounds.y + yOffset + 25, componentWidth, 25};
    Rectangle zRect = {dialogBounds.x + padding + 2*(componentWidth + padding), dialogBounds.y + yOffset + 25, componentWidth, 25};
    
    // X component
    GuiLabel({xRect.x, xRect.y - 18, componentWidth, 15}, "X");
    if (GuiTextBox(xRect, xBuffer, sizeof(xBuffer), true)) {
        try {
            state.vectorValue.x = std::stof(xBuffer);
            state.isDirty = true;
        } catch (const std::exception&) {}
    }
    
    // Y component
    GuiLabel({yRect.x, yRect.y - 18, componentWidth, 15}, "Y");
    if (GuiTextBox(yRect, yBuffer, sizeof(yBuffer), true)) {
        try {
            state.vectorValue.y = std::stof(yBuffer);
            state.isDirty = true;
        } catch (const std::exception&) {}
    }
    
    // Z component
    GuiLabel({zRect.x, zRect.y - 18, componentWidth, 15}, "Z");
    if (GuiTextBox(zRect, zBuffer, sizeof(zBuffer), true)) {
        try {
            state.vectorValue.z = std::stof(zBuffer);
            state.isDirty = true;
        } catch (const std::exception&) {}
    }
    
    return yOffset + fieldHeight;
}

float ScriptingEditor::drawComboField(const FieldDefinition& field, FieldState& state, Rectangle dialogBounds, float yOffset) {
    const float fieldHeight = 50.0f;
    const float padding = 10.0f;
    
    // Draw label
    Rectangle labelRect = {dialogBounds.x + padding, dialogBounds.y + yOffset, dialogBounds.width - 2*padding, 20};
    GuiLabel(labelRect, field.label.c_str());
    
    // Build options string for GuiComboBox
    std::string optionsStr = "";
    for (size_t i = 0; i < field.options.size(); ++i) {
        if (i > 0) optionsStr += ";";
        optionsStr += field.options[i];
    }
    
    // Draw combo box
    Rectangle comboRect = {dialogBounds.x + padding, dialogBounds.y + yOffset + 25, dialogBounds.width - 2*padding, 25};
    int newIndex = GuiComboBox(comboRect, optionsStr.c_str(), &state.comboIndex);
    
    if (newIndex != state.comboIndex) {
        state.comboIndex = newIndex;
        if (state.comboIndex >= 0 && state.comboIndex < field.options.size()) {
            state.setFromString(field.options[state.comboIndex]);
        }
    }
    
    return yOffset + fieldHeight;
}

float ScriptingEditor::drawIntegerField(const FieldDefinition& field, FieldState& state, Rectangle dialogBounds, float yOffset) {
    const float fieldHeight = 50.0f;
    const float padding = 10.0f;
    
    // Draw label
    Rectangle labelRect = {dialogBounds.x + padding, dialogBounds.y + yOffset, dialogBounds.width - 2*padding, 20};
    GuiLabel(labelRect, field.label.c_str());
    
    // Draw input field
    Rectangle inputRect = {dialogBounds.x + padding, dialogBounds.y + yOffset + 25, dialogBounds.width - 2*padding, 25};
    
    // Update text buffer if value changed externally
    if (state.intValue != static_cast<int>(std::stof(state.stringValue))) {
        state.setFromInt(state.intValue);
    }
    
    if (GuiTextBox(inputRect, state.textBuffer, sizeof(state.textBuffer), true)) {
        try {
            state.intValue = std::stoi(state.textBuffer);
            state.floatValue = static_cast<float>(state.intValue);
            state.stringValue = state.textBuffer;
            
            // Clamp to range if specified
            if (field.minValue != field.maxValue) {
                state.intValue = std::max(static_cast<int>(field.minValue), 
                                        std::min(static_cast<int>(field.maxValue), state.intValue));
                state.setFromInt(state.intValue);
            }
            
            state.isDirty = true;
        } catch (const std::exception&) {
            // Invalid input, revert to previous value
            state.setFromInt(state.intValue);
        }
    }
    
    // Show range info if specified
    if (field.minValue != field.maxValue) {
        Rectangle rangeRect = {dialogBounds.x + padding, dialogBounds.y + yOffset + 52, dialogBounds.width - 2*padding, 15};
        std::string rangeText = "Range: " + std::to_string(static_cast<int>(field.minValue)) + 
                               " - " + std::to_string(static_cast<int>(field.maxValue));
        GuiLabel(rangeRect, rangeText.c_str());
        return yOffset + fieldHeight + 20;
    }
    
    return yOffset + fieldHeight;
}

