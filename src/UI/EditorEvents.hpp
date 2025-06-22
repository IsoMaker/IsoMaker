#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <string>
#include <variant>
#include "raylib.h"

namespace UI {

// Event types for different editor actions
enum class EditorEventType {
    // Tool events
    TOOL_CHANGED,
    TOOL_USED,
    
    // Object events
    OBJECT_SELECTED,
    OBJECT_CREATED,
    OBJECT_DELETED,
    OBJECT_MODIFIED,
    
    // View events
    CAMERA_MOVED,
    ZOOM_CHANGED,
    GRID_TOGGLED,
    
    // File events
    FILE_NEW,
    FILE_OPEN,
    FILE_SAVE,
    FILE_EXPORT,
    
    // Editor mode events
    EDITOR_MODE_CHANGED,
    
    // Asset events
    ASSET_SELECTED,
    ASSET_LOADED,
    
    // Scene synchronization events
    SCENE_UPDATED,
    SCENE_OBJECT_ADDED,
    SCENE_OBJECT_REMOVED,
    SCENE_OBJECT_RENAMED
};

// Event data variants to hold different types of event payload
using EventData = std::variant<
    int,                    // For tool indices, object IDs, etc.
    float,                  // For zoom levels, distances, etc.
    std::string,            // For file paths, object names, etc.
    Vector3,                // For positions, rotations, scale
    Vector2,                // For 2D positions, cursor coords
    bool                    // For toggles, states
>;

// Event structure
struct EditorEvent {
    EditorEventType type;
    EventData data;
    std::string description;
    
    EditorEvent(EditorEventType t, EventData d = {}, const std::string& desc = "")
        : type(t), data(d), description(desc) {}
};

// Event handler function type
using EventHandler = std::function<void(const EditorEvent&)>;

// Event dispatcher class
class EventDispatcher {
public:
    // Subscribe to events
    void subscribe(EditorEventType eventType, EventHandler handler);
    
    // Dispatch an event to all subscribers
    void dispatch(const EditorEvent& event);
    
    // Remove all handlers for a specific event type
    void unsubscribe(EditorEventType eventType);
    
    // Clear all handlers
    void clear();

private:
    std::unordered_map<EditorEventType, std::vector<EventHandler>> _handlers;
};

// Global event dispatcher instance
extern EventDispatcher g_eventDispatcher;

// Convenience functions for common events
namespace Events {
    void toolChanged(int toolIndex);
    void objectSelected(int objectId);
    void objectCreated(const Vector3& position);
    void objectDeleted(int objectId);
    void cameraMove(const Vector3& position);
    void zoomChanged(float zoomLevel);
    void gridToggled(bool enabled);
    void fileAction(EditorEventType type, const std::string& filepath = "");
    void editorModeChanged(int modeIndex);
    void assetSelected(int assetIndex);
    
    // Scene synchronization events
    void sceneUpdated();
    void sceneObjectAdded(int objectId);
    void sceneObjectRemoved(int objectId);
    void sceneObjectRenamed(int objectId, const std::string& newName);
}

} // namespace UI