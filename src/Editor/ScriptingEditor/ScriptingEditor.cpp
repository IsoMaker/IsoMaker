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
#include <cstring>

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
    
    // Calculate panel layouts
    Rectangle leftPanel = {
        mainViewArea.x,
        mainViewArea.y,
        _leftPanelWidth,
        mainViewArea.height
    };
    
    Rectangle canvasPanel = {
        mainViewArea.x + _leftPanelWidth + _panelPadding,
        mainViewArea.y,
        mainViewArea.width - _leftPanelWidth - _panelPadding,
        mainViewArea.height
    };
    
    // Draw panels
    drawBlockPalette(leftPanel);
    drawScriptCanvas(canvasPanel);
    
    // Draw drag preview if dragging from palette
    if (_isDraggingFromPalette) {
        ScriptBlock previewBlock(0, _draggedBlockType, _currentMousePos);
        previewBlock.color.a = 150; // Semi-transparent
        drawBlock(previewBlock, {-previewBlock.size.x/2, -previewBlock.size.y/2}, true);
    }
    
    // Draw configuration dialog on top if open
    if (_showConfigDialog) {
        drawConfigDialog();
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
        BlockType::IF, BlockType::IF_ELSE, BlockType::WHILE
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
            color = {52, 144, 220, 255}; // Blue
            break;
        case BlockType::ON_CLICK:
            label = "OnClick";
            color = {52, 144, 220, 255}; // Blue
            break;
        case BlockType::ON_UPDATE:
            label = "OnUpdate";
            color = {52, 144, 220, 255}; // Blue
            break;
        case BlockType::ON_KEY_PRESS:
            label = "On key press Space";
            color = {52, 144, 220, 255}; // Blue
            size = {160, 40}; // Wider for longer text
            break;
            
        // Actions - Green
        case BlockType::MOVE:
            label = "Move";
            color = {40, 167, 69, 255}; // Green
            break;
        case BlockType::ROTATE:
            label = "Rotate";
            color = {40, 167, 69, 255}; // Green
            break;
        case BlockType::CHANGE_COLOR:
            label = "Change Color";
            color = {40, 167, 69, 255}; // Green
            size = {140, 40}; // Wider for longer text
            break;
        case BlockType::HIDE:
            label = "Hide";
            color = {40, 167, 69, 255}; // Green
            break;
        case BlockType::SHOW:
            label = "Show";
            color = {40, 167, 69, 255}; // Green
            break;
            
        // Conditions - Orange
        case BlockType::IF:
            label = "If";
            color = {255, 87, 34, 255}; // Orange
            break;
        case BlockType::IF_ELSE:
            label = "If Else";
            color = {255, 87, 34, 255}; // Orange
            break;
        case BlockType::WHILE:
            label = "While";
            color = {255, 87, 34, 255}; // Orange
            break;
            
        // Miscellaneous - Gray
        case BlockType::TRUE:
            label = "True";
            color = {108, 117, 125, 255}; // Gray
            break;
        case BlockType::FALSE:
            label = "False";
            color = {108, 117, 125, 255}; // Gray
            break;
        case BlockType::VALUE:
            label = "Value";
            color = {108, 117, 125, 255}; // Gray
            break;
        case BlockType::ENTITY:
            label = "Entity";
            color = {108, 117, 125, 255}; // Gray
            break;
        case BlockType::DELAY:
            label = "Delay";
            color = {108, 117, 125, 255}; // Gray
            break;
        case BlockType::LOG:
            label = "Log";
            color = {108, 117, 125, 255}; // Gray
            break;
    }
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
        
        drawBlock(paletteBlock, {0, 0}, false);
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
        
        // Draw all canvas blocks
        for (const ScriptBlock* block : sortedBlocks) {
            drawBlock(*block, _canvasOffset, true);
            
            // Draw connection points if this block is highlighted for connection
            if (block->isHighlightedForConnection) {
                drawConnectionPoints(*block);
            }
        }
    }
    
    // Draw canvas border
    DrawRectangleLinesEx(canvasArea, 1, UI::UI_SECONDARY);
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
    DrawRectangleRec(messageArea, Color{255, 255, 255, 200});
    DrawRectangleLinesEx(messageArea, 1, UI::UI_PRIMARY);
    
    GuiLabel(messageArea, "Select a scene object on the right to start scripting.");
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

void ScriptingEditor::drawBlock(const ScriptBlock& block, Vector2 offset, bool shadow) {
    Vector2 drawPos = {block.position.x + offset.x, block.position.y + offset.y};
    Rectangle blockRect = {drawPos.x, drawPos.y, block.size.x, block.size.y};
    
    // Draw shadow if requested
    if (shadow) {
        Rectangle shadowRect = {drawPos.x + 3, drawPos.y + 3, block.size.x, block.size.y};
        DrawRectangleRounded(shadowRect, 0.2f, 6, Color{0, 0, 0, 50});
    }
    
    // Draw block background
    DrawRectangleRounded(blockRect, 0.2f, 6, block.color);
    
    // Draw border with enhanced styling
    Color borderColor = UI::UI_PRIMARY;
    if (block.isDragging) {
        borderColor = Color{255, 255, 255, 255}; // White border when dragging
    }
    DrawRectangleRoundedLinesEx(blockRect, 0.2f, 6, 2.0f, borderColor);
    
    // Get display label (with configuration)
    std::string displayText = block.isOnCanvas ? block.getDisplayLabel() : block.label;
    
    // Draw block label with better typography
    Vector2 textSize = MeasureTextEx(GetFontDefault(), displayText.c_str(), 14, 1);
    Vector2 textPos = {
        drawPos.x + (block.size.x - textSize.x) / 2,
        drawPos.y + (block.size.y - textSize.y) / 2
    };
    
    // Draw text shadow for better readability
    DrawTextEx(GetFontDefault(), displayText.c_str(), 
               Vector2{textPos.x + 1, textPos.y + 1}, 14, 1, Color{0, 0, 0, 100});
    
    // Draw main text
    DrawTextEx(GetFontDefault(), displayText.c_str(), textPos, 14, 1, WHITE);
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
            std::cout << "[ScriptingEditor] Started dragging canvas block: " << canvasBlock->label << std::endl;
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
        
        // Update connection system
        updateBlockConnections();
        
        // If block has connections, move connected blocks
        Vector2 deltaPos = {_draggedCanvasBlock->position.x - oldPos.x, 
                           _draggedCanvasBlock->position.y - oldPos.y};
        if (deltaPos.x != 0 || deltaPos.y != 0) {
            updateConnectedBlockPositions(_draggedCanvasBlock->id, deltaPos);
        }
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
            // Remove block and all its connections
            int blockId = _draggedCanvasBlock->id;
            removeAllConnectionsForBlock(blockId);
            removeBlockFromCanvas(blockId);
            std::cout << "[ScriptingEditor] Removed block from canvas" << std::endl;
        } else {
            // Check if we should create a connection
            if (_connectionCandidate) {
                // Determine connection direction
                if (_draggedCanvasBlock->position.y < _connectionCandidate->position.y) {
                    // Dragged block is above - connect output to input
                    createConnection(_draggedCanvasBlock->id, _connectionCandidate->id);
                } else {
                    // Dragged block is below - connect input to output
                    createConnection(_connectionCandidate->id, _draggedCanvasBlock->id);
                }
            }
        }
        
        // Clear connection highlights
        updateBlockConnections();
        
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
    
    std::cout << "[ScriptingEditor] Added block '" << newBlock.label << "' to object " << selectedObjId << std::endl;
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
    return tempBlock.label;
}

Color ScriptingEditor::getBlockColor(BlockType type) const {
    ScriptBlock tempBlock(0, type);
    return tempBlock.color;
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
            openConfigDialog(block);
        }
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
    
    Rectangle dialogBounds = {300, 200, 400, 300};
    
    // Draw dialog background
    DrawRectangleRec(dialogBounds, UI::PANEL_BACKGROUND);
    DrawRectangleLinesEx(dialogBounds, 2, UI::UI_PRIMARY);
    
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

// Connection management implementation
void ScriptingEditor::updateBlockConnections() {
    if (!_isDraggingCanvasBlock || !_draggedCanvasBlock) {
        // Clear previous connection highlights
        int selectedObjId = getSelectedObjectId();
        if (selectedObjId != -1) {
            auto it = _objectScripts.find(selectedObjId);
            if (it != _objectScripts.end()) {
                for (auto& block : it->second.blocks) {
                    block.isHighlightedForConnection = false;
                }
            }
        }
        _connectionCandidate = nullptr;
        _showConnectionPreview = false;
        return;
    }
    
    // Find potential connection candidate
    ScriptBlock* candidate = findConnectionCandidate(*_draggedCanvasBlock);
    _connectionCandidate = candidate;
    
    if (candidate) {
        candidate->isHighlightedForConnection = true;
        _showConnectionPreview = true;
        
        // Determine connection points based on relative positions
        if (_draggedCanvasBlock->position.y < candidate->position.y) {
            // Dragged block is above - connect output to input
            _connectionPreviewStart = _draggedCanvasBlock->getOutputConnectionPoint();
            _connectionPreviewEnd = candidate->getInputConnectionPoint();
        } else {
            // Dragged block is below - connect input to output
            _connectionPreviewStart = candidate->getOutputConnectionPoint();
            _connectionPreviewEnd = _draggedCanvasBlock->getInputConnectionPoint();
        }
    } else {
        _showConnectionPreview = false;
    }
}

void ScriptingEditor::createConnection(int fromBlockId, int toBlockId) {
    int selectedObjId = getSelectedObjectId();
    if (selectedObjId == -1) return;
    
    auto it = _objectScripts.find(selectedObjId);
    if (it == _objectScripts.end()) return;
    
    // Find the blocks
    ScriptBlock* fromBlock = nullptr;
    ScriptBlock* toBlock = nullptr;
    
    for (auto& block : it->second.blocks) {
        if (block.id == fromBlockId) fromBlock = &block;
        if (block.id == toBlockId) toBlock = &block;
    }
    
    if (!fromBlock || !toBlock || !canBlocksConnect(*fromBlock, *toBlock)) {
        return;
    }
    
    // Remove any existing connections for these blocks
    removeConnection(fromBlockId, -1); // Remove fromBlock's output connection
    removeConnection(-1, toBlockId);   // Remove toBlock's input connection
    
    // Create new connection
    Vector2 fromPoint = fromBlock->getOutputConnectionPoint();
    Vector2 toPoint = toBlock->getInputConnectionPoint();
    
    BlockConnection newConnection(fromBlockId, toBlockId, fromPoint, toPoint);
    it->second.connections.push_back(newConnection);
    
    // Update block connection states
    fromBlock->hasOutputConnection = true;
    fromBlock->connectedToId = toBlockId;
    toBlock->hasInputConnection = true;
    toBlock->connectedFromId = fromBlockId;
    
    std::cout << "[ScriptingEditor] Created connection from block " << fromBlockId << " to block " << toBlockId << std::endl;
}

void ScriptingEditor::removeConnection(int fromBlockId, int toBlockId) {
    int selectedObjId = getSelectedObjectId();
    if (selectedObjId == -1) return;
    
    auto it = _objectScripts.find(selectedObjId);
    if (it == _objectScripts.end()) return;
    
    auto& connections = it->second.connections;
    
    // Remove matching connections
    connections.erase(
        std::remove_if(connections.begin(), connections.end(),
            [fromBlockId, toBlockId](const BlockConnection& conn) {
                return (fromBlockId == -1 || conn.fromBlockId == fromBlockId) &&
                       (toBlockId == -1 || conn.toBlockId == toBlockId);
            }),
        connections.end()
    );
    
    // Update block connection states
    for (auto& block : it->second.blocks) {
        if (fromBlockId == -1 || block.id == fromBlockId) {
            if (toBlockId == -1 || block.connectedToId == toBlockId) {
                block.hasOutputConnection = false;
                block.connectedToId = -1;
            }
        }
        if (toBlockId == -1 || block.id == toBlockId) {
            if (fromBlockId == -1 || block.connectedFromId == fromBlockId) {
                block.hasInputConnection = false;
                block.connectedFromId = -1;
            }
        }
    }
}

void ScriptingEditor::removeAllConnectionsForBlock(int blockId) {
    removeConnection(blockId, -1); // Remove as source
    removeConnection(-1, blockId); // Remove as target
}

ScriptBlock* ScriptingEditor::findConnectionCandidate(const ScriptBlock& draggedBlock) {
    int selectedObjId = getSelectedObjectId();
    if (selectedObjId == -1) return nullptr;
    
    auto it = _objectScripts.find(selectedObjId);
    if (it == _objectScripts.end()) return nullptr;
    
    ScriptBlock* closestBlock = nullptr;
    float closestDistance = _connectionSnapDistance;
    
    for (auto& block : it->second.blocks) {
        if (block.id == draggedBlock.id || !block.isOnCanvas) continue;
        
        if (canBlocksConnect(draggedBlock, block)) {
            float distance = getDistanceBetweenConnectionPoints(draggedBlock, block);
            if (distance < closestDistance) {
                closestDistance = distance;
                closestBlock = &block;
            }
        }
    }
    
    return closestBlock;
}

void ScriptingEditor::updateConnectedBlockPositions(int movedBlockId, Vector2 deltaPos) {
    int selectedObjId = getSelectedObjectId();
    if (selectedObjId == -1) return;
    
    auto it = _objectScripts.find(selectedObjId);
    if (it == _objectScripts.end()) return;
    
    // Find all blocks connected to the moved block and move them recursively
    std::vector<int> blocksToMove;
    
    // Find blocks connected as outputs
    for (const auto& connection : it->second.connections) {
        if (connection.fromBlockId == movedBlockId) {
            blocksToMove.push_back(connection.toBlockId);
        }
    }
    
    // Move connected blocks
    for (int blockId : blocksToMove) {
        for (auto& block : it->second.blocks) {
            if (block.id == blockId) {
                block.position.x += deltaPos.x;
                block.position.y += deltaPos.y;
                // Recursively move blocks connected to this one
                updateConnectedBlockPositions(blockId, deltaPos);
                break;
            }
        }
    }
}

bool ScriptingEditor::canBlocksConnect(const ScriptBlock& from, const ScriptBlock& to) {
    // Basic connection rules:
    // 1. Blocks cannot connect to themselves
    // 2. Event blocks (like OnStart) can connect to action blocks
    // 3. Action blocks can connect to other action blocks or condition blocks
    // 4. Condition blocks can connect to action blocks
    // 5. A block can only have one input and one output connection
    
    if (from.id == to.id) return false;
    
    // Check if connection would create a cycle (simplified check)
    if (from.connectedFromId == to.id) return false;
    
    // Check if either block already has the required connection type
    bool connectingFromOutput = from.position.y < to.position.y;
    if (connectingFromOutput) {
        if (from.hasOutputConnection || to.hasInputConnection) return false;
    } else {
        if (to.hasOutputConnection || from.hasInputConnection) return false;
    }
    
    return true;
}

float ScriptingEditor::getDistanceBetweenConnectionPoints(const ScriptBlock& from, const ScriptBlock& to) {
    Vector2 fromPoint, toPoint;
    
    // Determine connection direction based on relative positions
    if (from.position.y < to.position.y) {
        // from is above to - connect output to input
        fromPoint = from.getOutputConnectionPoint();
        toPoint = to.getInputConnectionPoint();
    } else {
        // from is below to - connect input to output  
        fromPoint = to.getOutputConnectionPoint();
        toPoint = from.getInputConnectionPoint();
    }
    
    float dx = fromPoint.x - toPoint.x;
    float dy = fromPoint.y - toPoint.y;
    return sqrt(dx * dx + dy * dy);
}

void ScriptingEditor::drawConnections(const VisualScript& script) {
    for (const auto& connection : script.connections) {
        // Update connection points in case blocks moved
        const ScriptBlock* fromBlock = nullptr;
        const ScriptBlock* toBlock = nullptr;
        
        for (const auto& block : script.blocks) {
            if (block.id == connection.fromBlockId) fromBlock = &block;
            if (block.id == connection.toBlockId) toBlock = &block;
        }
        
        if (fromBlock && toBlock) {
            Vector2 startPoint = fromBlock->getOutputConnectionPoint();
            Vector2 endPoint = toBlock->getInputConnectionPoint();
            
            // Apply canvas offset
            startPoint.x += _canvasOffset.x;
            startPoint.y += _canvasOffset.y;
            endPoint.x += _canvasOffset.x;
            endPoint.y += _canvasOffset.y;
            
            drawConnectionLine(startPoint, endPoint, UI::UI_PRIMARY, 3.0f);
        }
    }
    
    // Draw connection preview if showing
    if (_showConnectionPreview) {
        Vector2 previewStart = {_connectionPreviewStart.x + _canvasOffset.x, 
                               _connectionPreviewStart.y + _canvasOffset.y};
        Vector2 previewEnd = {_connectionPreviewEnd.x + _canvasOffset.x, 
                             _connectionPreviewEnd.y + _canvasOffset.y};
        drawConnectionLine(previewStart, previewEnd, Color{100, 255, 100, 200}, 2.0f);
    }
}

void ScriptingEditor::drawConnectionLine(Vector2 start, Vector2 end, Color color, float thickness) {
    // Draw a curved line for better visual appeal
    float midY = (start.y + end.y) / 2;
    Vector2 control1 = {start.x, midY};
    Vector2 control2 = {end.x, midY};
    
    // Draw bezier curve (approximated with line segments)
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
    if (length > 0) {
        direction.x /= length;
        direction.y /= length;
        
        Vector2 arrowLeft = {end.x - direction.x * 8 - direction.y * 4, 
                            end.y - direction.y * 8 + direction.x * 4};
        Vector2 arrowRight = {end.x - direction.x * 8 + direction.y * 4, 
                             end.y - direction.y * 8 - direction.x * 4};
        
        DrawLineEx(end, arrowLeft, thickness, color);
        DrawLineEx(end, arrowRight, thickness, color);
    }
}

void ScriptingEditor::drawConnectionPoints(const ScriptBlock& block) {
    Vector2 inputPoint = block.getInputConnectionPoint();
    Vector2 outputPoint = block.getOutputConnectionPoint();
    
    // Apply canvas offset
    inputPoint.x += _canvasOffset.x;
    inputPoint.y += _canvasOffset.y;
    outputPoint.x += _canvasOffset.x;
    outputPoint.y += _canvasOffset.y;
    
    // Draw connection points as small circles
    Color inputColor = block.hasInputConnection ? Color{255, 100, 100, 255} : Color{150, 150, 150, 255};
    Color outputColor = block.hasOutputConnection ? Color{100, 255, 100, 255} : Color{150, 150, 150, 255};
    
    DrawCircle(inputPoint.x, inputPoint.y, 4, inputColor);
    DrawCircle(outputPoint.x, outputPoint.y, 4, outputColor);
    
    // Draw white outline
    DrawCircleLines(inputPoint.x, inputPoint.y, 4, WHITE);
    DrawCircleLines(outputPoint.x, outputPoint.y, 4, WHITE);
}
