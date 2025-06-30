/**
 * @file ScriptingEditor.cpp
 * @brief Implementation of the ScriptingEditor class
 * @author IsoMaker Team
 * @version 0.1
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
    _initialized = true;
    
    std::cout << "[ScriptingEditor] Initialized successfully" << std::endl;
}

void ScriptingEditor::update(input::IHandlerBase &inputHandler) {
    if (!_initialized) {
        return;
    }
    
    // Placeholder update logic - will be expanded with scripting functionality
}

void ScriptingEditor::draw(Rectangle mainViewArea) {
    if (!_initialized) {
        return;
    }
    
    // Clear the main view area with background
    DrawRectangleRec(mainViewArea, UI::BACKGROUND);
    
    // Calculate panel layouts
    Rectangle leftPanel = {
        mainViewArea.x,
        mainViewArea.y,
        _leftPanelWidth,
        mainViewArea.height
    };
    
    Rectangle mainPanel = {
        mainViewArea.x + _leftPanelWidth + _panelPadding,
        mainViewArea.y,
        mainViewArea.width - _leftPanelWidth - _panelPadding,
        mainViewArea.height
    };
    
    // Draw panels
    drawLeftPanel(leftPanel);
    drawMainPanel(mainPanel);
    
    // Draw dialogs on top
    if (_showAddScriptDialog) {
        drawAddScriptDialog();
    }
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
    return 0; // Placeholder - no scripts yet
}

int ScriptingEditor::getSelectedObjectId() const {
    // Forward to the current scene provider (usually the Map Editor)
    if (_currentSceneProvider) {
        return _currentSceneProvider->getSelectedObjectId();
    }
    return _selectedScriptId;
}

std::string ScriptingEditor::getSelectedScriptName() const {
    if (_selectedScriptId == -1) {
        return "";
    }
    return "Script_" + std::to_string(_selectedScriptId); // Placeholder
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
    _selectedScriptId = objectId;
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

void ScriptingEditor::drawLeftPanel(Rectangle bounds) {
    // Draw panel background
    DrawRectangleRec(bounds, UI::PANEL_BACKGROUND);
    DrawRectangleLinesEx(bounds, 1, UI::UI_PRIMARY);
    
    // Panel title
    Rectangle titleArea = {bounds.x + 5, bounds.y + 5, bounds.width - 10, 25};
    GuiLabel(titleArea, "Scene Objects");
    
    // Objects list area
    Rectangle listArea = {bounds.x + 5, bounds.y + 35, bounds.width - 10, bounds.height - 40};
    
    // Get scene objects from the scene provider
    std::vector<UI::SceneObjectInfo> objects = getSceneObjects();
    
    if (objects.empty()) {
        Rectangle emptyArea = {listArea.x, listArea.y + 20, listArea.width, 20};
        GuiLabel(emptyArea, "No objects in scene");
        return;
    }
    
    // Draw scrollable list of objects
    float itemHeight = 30.0f;
    float yOffset = 0.0f;
    
    for (size_t i = 0; i < objects.size(); i++) {
        const auto& obj = objects[i];
        Rectangle itemBounds = {listArea.x, listArea.y + yOffset, listArea.width, itemHeight};
        
        // Skip if item is outside visible area
        if (itemBounds.y + itemBounds.height < listArea.y || itemBounds.y > listArea.y + listArea.height) {
            yOffset += itemHeight;
            continue;
        }
        
        // Highlight selected object
        bool isSelected = (obj.id == _selectedObjectId);
        if (isSelected) {
            DrawRectangleRec(itemBounds, UI::SELECTED_BACKGROUND);
        }
        
        // Object button
        if (GuiButton(itemBounds, obj.name.c_str())) {
            handleObjectSelection(obj.id);
        }
        
        yOffset += itemHeight;
    }
}

void ScriptingEditor::drawMainPanel(Rectangle bounds) {
    // Draw panel background
    DrawRectangleRec(bounds, UI::PANEL_BACKGROUND);
    DrawRectangleLinesEx(bounds, 1, UI::UI_PRIMARY);
    
    if (_selectedObjectId == -1) {
        // No object selected - show instruction
        Rectangle messageArea = {
            bounds.x + bounds.width/2 - 150,
            bounds.y + bounds.height/2 - 10,
            300, 20
        };
        GuiLabel(messageArea, "Select an object to view or add scripts.");
        return;
    }
    
    // Get object info
    UI::SceneObjectInfo objInfo = getObjectInfo(_selectedObjectId);
    
    // Object title
    Rectangle titleArea = {bounds.x + 10, bounds.y + 10, bounds.width - 20, 25};
    std::string title = "Scripts for: " + objInfo.name;
    GuiLabel(titleArea, title.c_str());
    
    // Add script button
    Rectangle addButtonArea = {bounds.x + bounds.width - 120, bounds.y + 10, 100, 25};
    if (GuiButton(addButtonArea, "Add Script")) {
        openAddScriptDialog();
    }
    
    // Scripts list area
    Rectangle scriptsArea = {bounds.x + 10, bounds.y + 45, bounds.width - 20, bounds.height - 55};
    drawScriptList(_selectedObjectId, scriptsArea);
}

void ScriptingEditor::drawScriptList(int objectId, Rectangle bounds) {
    std::vector<AttachedScript>& scripts = getObjectScripts(objectId);
    
    if (scripts.empty()) {
        Rectangle emptyArea = {bounds.x, bounds.y + 20, bounds.width, 20};
        GuiLabel(emptyArea, "No scripts attached to this object.");
        return;
    }
    
    float itemHeight = 80.0f;
    float yOffset = 0.0f;
    
    for (size_t i = 0; i < scripts.size(); i++) {
        AttachedScript& script = scripts[i];
        Rectangle itemBounds = {bounds.x, bounds.y + yOffset, bounds.width, itemHeight};
        
        // Skip if outside visible area
        if (itemBounds.y + itemBounds.height < bounds.y || itemBounds.y > bounds.y + bounds.height) {
            yOffset += itemHeight;
            continue;
        }
        
        // Script item background
        DrawRectangleRec(itemBounds, UI::BACKGROUND);
        DrawRectangleLinesEx(itemBounds, 1, UI::UI_SECONDARY);
        
        // Script name and status
        Rectangle nameArea = {itemBounds.x + 10, itemBounds.y + 5, itemBounds.width - 120, 20};
        std::string nameText = script.name + (script.enabled ? " (Enabled)" : " (Disabled)");
        GuiLabel(nameArea, nameText.c_str());
        
        // Script type
        Rectangle typeArea = {itemBounds.x + 10, itemBounds.y + 25, itemBounds.width - 120, 15};
        std::string typeText = "Type: " + getTemplateDisplayName(script.config.templateType);
        GuiLabel(typeArea, typeText.c_str());
        
        // Script pseudocode preview
        Rectangle previewArea = {itemBounds.x + 10, itemBounds.y + 40, itemBounds.width - 120, 30};
        std::string pseudocode = generatePseudocode(script);
        GuiLabel(previewArea, pseudocode.c_str());
        
        // Control buttons
        Rectangle editButton = {itemBounds.x + itemBounds.width - 105, itemBounds.y + 5, 45, 20};
        Rectangle toggleButton = {itemBounds.x + itemBounds.width - 105, itemBounds.y + 30, 45, 20};
        Rectangle deleteButton = {itemBounds.x + itemBounds.width - 55, itemBounds.y + 5, 45, 20};
        
        if (GuiButton(editButton, "Edit")) {
            openConfigDialog(&script);
        }
        
        if (GuiButton(toggleButton, script.enabled ? "Disable" : "Enable")) {
            script.enabled = !script.enabled;
        }
        
        if (GuiButton(deleteButton, "Delete")) {
            removeScriptFromObject(objectId, script.scriptId);
            break; // Exit loop since we modified the vector
        }
        
        yOffset += itemHeight;
    }
}

void ScriptingEditor::drawAddScriptDialog() {
    Rectangle dialogBounds = {300, 200, 400, 300};
    
    // Dialog background
    DrawRectangleRec(dialogBounds, UI::PANEL_BACKGROUND);
    DrawRectangleLinesEx(dialogBounds, 2, UI::UI_PRIMARY);
    
    // Title
    Rectangle titleArea = {dialogBounds.x + 10, dialogBounds.y + 10, dialogBounds.width - 20, 25};
    GuiLabel(titleArea, "Add New Script");
    
    // Script name input
    Rectangle nameArea = {dialogBounds.x + 10, dialogBounds.y + 45, dialogBounds.width - 20, 25};
    GuiLabel((Rectangle){nameArea.x, nameArea.y - 20, nameArea.width, 20}, "Script Name:");
    GuiTextBox(nameArea, _newScriptName, 64, true);
    
    // Template selection
    Rectangle templateArea = {dialogBounds.x + 10, dialogBounds.y + 85, dialogBounds.width - 20, 25};
    GuiLabel((Rectangle){templateArea.x, templateArea.y - 20, templateArea.width, 20}, "Script Template:");
    
    const char* templateNames[] = {"Rotate", "Move", "Toggle Visibility", "Scale", "Blink", "Oscillate"};
    GuiComboBox(templateArea, "Rotate;Move;Toggle Visibility;Scale;Blink;Oscillate", &_selectedTemplateIndex);
    
    // Template description
    Rectangle descArea = {dialogBounds.x + 10, dialogBounds.y + 125, dialogBounds.width - 20, 60};
    std::string description;
    switch (_selectedTemplateIndex) {
        case 0: description = "Rotates the object around a specified axis at a given speed."; break;
        case 1: description = "Moves the object along a vector at a specified speed."; break;
        case 2: description = "Toggles the object's visibility on and off."; break;
        case 3: description = "Scales the object up and down over time."; break;
        case 4: description = "Makes the object blink by toggling visibility rapidly."; break;
        case 5: description = "Moves the object back and forth between two positions."; break;
    }
    GuiLabel(descArea, description.c_str());
    
    // Buttons
    Rectangle createButton = {dialogBounds.x + dialogBounds.width - 180, dialogBounds.y + dialogBounds.height - 40, 80, 30};
    Rectangle cancelButton = {dialogBounds.x + dialogBounds.width - 90, dialogBounds.y + dialogBounds.height - 40, 80, 30};
    
    if (GuiButton(createButton, "Create")) {
        if (_selectedObjectId != -1 && strlen(_newScriptName) > 0) {
            ScriptTemplate templateType = static_cast<ScriptTemplate>(_selectedTemplateIndex);
            addScriptToObject(_selectedObjectId, templateType, std::string(_newScriptName));
            closeAddScriptDialog();
        }
    }
    
    if (GuiButton(cancelButton, "Cancel")) {
        closeAddScriptDialog();
    }
}

void ScriptingEditor::drawConfigDialog() {
    if (!_editingScript) return;
    
    Rectangle dialogBounds = {250, 150, 500, 400};
    
    // Dialog background
    DrawRectangleRec(dialogBounds, UI::PANEL_BACKGROUND);
    DrawRectangleLinesEx(dialogBounds, 2, UI::UI_PRIMARY);
    
    // Title
    Rectangle titleArea = {dialogBounds.x + 10, dialogBounds.y + 10, dialogBounds.width - 20, 25};
    std::string title = "Configure: " + _editingScript->name;
    GuiLabel(titleArea, title.c_str());
    
    // Configuration fields based on script type
    float yOffset = 50;
    float fieldHeight = 30;
    float labelHeight = 20;
    
    switch (_editingScript->config.templateType) {
        case ScriptTemplate::ROTATE: {
            // Speed parameter
            Rectangle speedLabel = {dialogBounds.x + 10, dialogBounds.y + yOffset, dialogBounds.width - 20, labelHeight};
            GuiLabel(speedLabel, "Rotation Speed (degrees/second):");
            
            Rectangle speedField = {dialogBounds.x + 10, dialogBounds.y + yOffset + labelHeight, dialogBounds.width - 20, fieldHeight};
            static char speedText[32] = "90.0";
            GuiTextBox(speedField, speedText, 32, true);
            _editingScript->config.floatParams["speed"] = std::stof(speedText);
            
            yOffset += fieldHeight + labelHeight + 10;
            
            // Axis parameter
            Rectangle axisLabel = {dialogBounds.x + 10, dialogBounds.y + yOffset, dialogBounds.width - 20, labelHeight};
            GuiLabel(axisLabel, "Rotation Axis:");
            
            Rectangle axisField = {dialogBounds.x + 10, dialogBounds.y + yOffset + labelHeight, dialogBounds.width - 20, fieldHeight};
            static int axisIndex = 1; // Y-axis default
            GuiComboBox(axisField, "X-Axis;Y-Axis;Z-Axis", &axisIndex);
            
            Vector3 axis = {0, 1, 0}; // Default Y-axis
            if (axisIndex == 0) axis = {1, 0, 0}; // X-axis
            else if (axisIndex == 2) axis = {0, 0, 1}; // Z-axis
            _editingScript->config.vectorParams["axis"] = axis;
            
            break;
        }
        case ScriptTemplate::MOVE: {
            // Speed and direction parameters
            Rectangle speedLabel = {dialogBounds.x + 10, dialogBounds.y + yOffset, dialogBounds.width - 20, labelHeight};
            GuiLabel(speedLabel, "Movement Speed:");
            
            Rectangle speedField = {dialogBounds.x + 10, dialogBounds.y + yOffset + labelHeight, dialogBounds.width - 20, fieldHeight};
            static char moveSpeedText[32] = "1.0";
            GuiTextBox(speedField, moveSpeedText, 32, true);
            _editingScript->config.floatParams["speed"] = std::stof(moveSpeedText);
            
            yOffset += fieldHeight + labelHeight + 10;
            
            Rectangle dirLabel = {dialogBounds.x + 10, dialogBounds.y + yOffset, dialogBounds.width - 20, labelHeight};
            GuiLabel(dirLabel, "Direction Vector (X, Y, Z):");
            
            // X, Y, Z input fields
            float fieldWidth = (dialogBounds.width - 40) / 3;
            static char xText[16] = "1.0", yText[16] = "0.0", zText[16] = "0.0";
            
            Rectangle xField = {dialogBounds.x + 10, dialogBounds.y + yOffset + labelHeight, fieldWidth, fieldHeight};
            Rectangle yField = {dialogBounds.x + 15 + fieldWidth, dialogBounds.y + yOffset + labelHeight, fieldWidth, fieldHeight};
            Rectangle zField = {dialogBounds.x + 20 + 2*fieldWidth, dialogBounds.y + yOffset + labelHeight, fieldWidth, fieldHeight};
            
            GuiTextBox(xField, xText, 16, true);
            GuiTextBox(yField, yText, 16, true);
            GuiTextBox(zField, zText, 16, true);
            
            _editingScript->config.vectorParams["direction"] = {std::stof(xText), std::stof(yText), std::stof(zText)};
            
            break;
        }
        case ScriptTemplate::TOGGLE_VISIBILITY: {
            // Interval parameter
            Rectangle intervalLabel = {dialogBounds.x + 10, dialogBounds.y + yOffset, dialogBounds.width - 20, labelHeight};
            GuiLabel(intervalLabel, "Toggle Interval (seconds):");
            
            Rectangle intervalField = {dialogBounds.x + 10, dialogBounds.y + yOffset + labelHeight, dialogBounds.width - 20, fieldHeight};
            static char intervalText[32] = "1.0";
            GuiTextBox(intervalField, intervalText, 32, true);
            _editingScript->config.floatParams["interval"] = std::stof(intervalText);
            
            break;
        }
        // Add more template configurations as needed
        default:
            break;
    }
    
    // Buttons
    Rectangle saveButton = {dialogBounds.x + dialogBounds.width - 180, dialogBounds.y + dialogBounds.height - 40, 80, 30};
    Rectangle cancelButton = {dialogBounds.x + dialogBounds.width - 90, dialogBounds.y + dialogBounds.height - 40, 80, 30};
    
    if (GuiButton(saveButton, "Save")) {
        closeConfigDialog();
    }
    
    if (GuiButton(cancelButton, "Cancel")) {
        closeConfigDialog();
    }
}

// Script management helper methods
void ScriptingEditor::addScriptToObject(int objectId, ScriptTemplate templateType, const std::string& name) {
    AttachedScript newScript(_nextScriptId++, name, templateType);
    
    // Set default parameters based on template type
    switch (templateType) {
        case ScriptTemplate::ROTATE:
            newScript.config.floatParams["speed"] = 90.0f;
            newScript.config.vectorParams["axis"] = {0.0f, 1.0f, 0.0f};
            break;
        case ScriptTemplate::MOVE:
            newScript.config.floatParams["speed"] = 1.0f;
            newScript.config.vectorParams["direction"] = {1.0f, 0.0f, 0.0f};
            break;
        case ScriptTemplate::TOGGLE_VISIBILITY:
            newScript.config.floatParams["interval"] = 1.0f;
            break;
        case ScriptTemplate::SCALE:
            newScript.config.floatParams["scale_min"] = 0.5f;
            newScript.config.floatParams["scale_max"] = 2.0f;
            newScript.config.floatParams["duration"] = 2.0f;
            break;
        case ScriptTemplate::BLINK:
            newScript.config.floatParams["interval"] = 0.5f;
            break;
        case ScriptTemplate::OSCILLATE:
            newScript.config.floatParams["distance"] = 2.0f;
            newScript.config.floatParams["speed"] = 1.0f;
            newScript.config.vectorParams["direction"] = {1.0f, 0.0f, 0.0f};
            break;
    }
    
    _objectScripts[objectId].push_back(newScript);
    std::cout << "[ScriptingEditor] Added script '" << name << "' to object " << objectId << std::endl;
}

void ScriptingEditor::removeScriptFromObject(int objectId, int scriptId) {
    auto& scripts = _objectScripts[objectId];
    scripts.erase(
        std::remove_if(scripts.begin(), scripts.end(),
            [scriptId](const AttachedScript& script) { return script.scriptId == scriptId; }),
        scripts.end()
    );
    std::cout << "[ScriptingEditor] Removed script " << scriptId << " from object " << objectId << std::endl;
}

std::vector<AttachedScript>& ScriptingEditor::getObjectScripts(int objectId) {
    return _objectScripts[objectId];
}

std::string ScriptingEditor::getTemplateDisplayName(ScriptTemplate templateType) const {
    switch (templateType) {
        case ScriptTemplate::ROTATE: return "Rotate";
        case ScriptTemplate::MOVE: return "Move";
        case ScriptTemplate::TOGGLE_VISIBILITY: return "Toggle Visibility";
        case ScriptTemplate::SCALE: return "Scale";
        case ScriptTemplate::BLINK: return "Blink";
        case ScriptTemplate::OSCILLATE: return "Oscillate";
        default: return "Unknown";
    }
}

std::string ScriptingEditor::generatePseudocode(const AttachedScript& script) const {
    switch (script.config.templateType) {
        case ScriptTemplate::ROTATE: {
            auto speedIt = script.config.floatParams.find("speed");
            float speed = (speedIt != script.config.floatParams.end()) ? speedIt->second : 90.0f;
            return "Rotate around Y-axis at " + std::to_string((int)speed) + "°/sec";
        }
        case ScriptTemplate::MOVE: {
            auto speedIt = script.config.floatParams.find("speed");
            float speed = (speedIt != script.config.floatParams.end()) ? speedIt->second : 1.0f;
            return "Move at speed " + std::to_string(speed) + " units/sec";
        }
        case ScriptTemplate::TOGGLE_VISIBILITY: {
            auto intervalIt = script.config.floatParams.find("interval");
            float interval = (intervalIt != script.config.floatParams.end()) ? intervalIt->second : 1.0f;
            return "Toggle visibility every " + std::to_string(interval) + " seconds";
        }
        case ScriptTemplate::SCALE: {
            return "Scale object up and down over time";
        }
        case ScriptTemplate::BLINK: {
            return "Blink object rapidly";
        }
        case ScriptTemplate::OSCILLATE: {
            return "Oscillate object back and forth";
        }
        default:
            return "Unknown script type";
    }
}

// UI interaction helper methods
void ScriptingEditor::handleObjectSelection(int objectId) {
    _selectedObjectId = objectId;
    selectObject(objectId); // Forward to scene provider
    std::cout << "[ScriptingEditor] Selected object for scripting: " << objectId << std::endl;
}

void ScriptingEditor::openAddScriptDialog() {
    _showAddScriptDialog = true;
    strcpy(_newScriptName, "New Script");
    _selectedTemplateIndex = 0;
}

void ScriptingEditor::closeAddScriptDialog() {
    _showAddScriptDialog = false;
}

void ScriptingEditor::openConfigDialog(AttachedScript* script) {
    _showConfigDialog = true;
    _editingScript = script;
}

void ScriptingEditor::closeConfigDialog() {
    _showConfigDialog = false;
    _editingScript = nullptr;
}