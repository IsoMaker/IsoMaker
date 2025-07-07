#include "EditorEvents.hpp"
#include <iostream>

namespace UI {

// Global event dispatcher instance
EventDispatcher g_eventDispatcher;

void EventDispatcher::subscribe(EditorEventType eventType, EventHandler handler) {
    _handlers[eventType].push_back(handler);
}

void EventDispatcher::dispatch(const EditorEvent& event) {
    auto it = _handlers.find(event.type);
    if (it != _handlers.end()) {
        for (const auto& handler : it->second) {
            handler(event);
        }
    }
}

void EventDispatcher::unsubscribe(EditorEventType eventType) {
    _handlers.erase(eventType);
}

void EventDispatcher::clear() {
    _handlers.clear();
}

// Convenience functions for common events
namespace Events {
    void toolChanged(int toolIndex) {
        g_eventDispatcher.dispatch(EditorEvent(EditorEventType::TOOL_CHANGED, toolIndex, "Tool changed"));
    }
    
    void objectSelected(int objectId) {
        g_eventDispatcher.dispatch(EditorEvent(EditorEventType::OBJECT_SELECTED, objectId, "Object selected"));
    }
    
    void objectCreated(const Vector3& position) {
        g_eventDispatcher.dispatch(EditorEvent(EditorEventType::OBJECT_CREATED, position, "Object created"));
    }
    
    void objectDeleted(int objectId) {
        g_eventDispatcher.dispatch(EditorEvent(EditorEventType::OBJECT_DELETED, objectId, "Object deleted"));
    }
    
    void cameraMove(const Vector3& position) {
        g_eventDispatcher.dispatch(EditorEvent(EditorEventType::CAMERA_MOVED, position, "Camera moved"));
    }
    
    void zoomChanged(float zoomLevel) {
        g_eventDispatcher.dispatch(EditorEvent(EditorEventType::ZOOM_CHANGED, zoomLevel, "Zoom changed"));
    }
    
    void gridToggled(bool enabled) {
        g_eventDispatcher.dispatch(EditorEvent(EditorEventType::GRID_TOGGLED, enabled, "Grid toggled"));
    }
    
    void fileAction(EditorEventType type, const std::string& filepath) {
        g_eventDispatcher.dispatch(EditorEvent(type, filepath, "File action"));
    }
    
    void editorModeChanged(int modeIndex) {
        g_eventDispatcher.dispatch(EditorEvent(EditorEventType::EDITOR_MODE_CHANGED, modeIndex, "Editor mode changed"));
    }
    
    void assetSelected(std::shared_ptr<AAsset> asset) {
        g_eventDispatcher.dispatch(EditorEvent(EditorEventType::ASSET_SELECTED, asset, "Asset selected"));
    }
    
    // Scene synchronization events
    void sceneUpdated() {
        g_eventDispatcher.dispatch(EditorEvent(EditorEventType::SCENE_UPDATED, 0, "Scene updated"));
    }
    
    void sceneObjectAdded(int objectId) {
        g_eventDispatcher.dispatch(EditorEvent(EditorEventType::SCENE_OBJECT_ADDED, objectId, "Scene object added"));
    }
    
    void sceneObjectRemoved(int objectId) {
        g_eventDispatcher.dispatch(EditorEvent(EditorEventType::SCENE_OBJECT_REMOVED, objectId, "Scene object removed"));
    }
    
    void sceneObjectRenamed(int objectId, const std::string& newName) {
        g_eventDispatcher.dispatch(EditorEvent(EditorEventType::SCENE_OBJECT_RENAMED, newName, "Scene object renamed"));
    }

    void addAssetRequested() {
        std::cout << "trigger event\n";
        g_eventDispatcher.dispatch(EditorEvent(EditorEventType::ASSET_LOADED, 0, "Add New Asset"));
    }
}

} // namespace UI
