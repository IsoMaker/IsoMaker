/**
 * @file EditorEvents.hpp
 * @brief Event-driven communication system for IsoMaker editor components
 * @author IsoMaker Team
 * @version 0.1
 */

#pragma once

#include <functional>
#include <unordered_map>
#include <vector>
#include <string>
#include <variant>
#include "raylib.h"

#include "Entities/BasicObject.hpp"

namespace UI {

/**
 * @brief Editor event types enumeration
 * 
 * Defines all types of events that can be dispatched between UI components and editors.
 * This event system enables loose coupling between different parts of the application
 * while maintaining synchronized state and responsive user interactions.
 * 
 * Events are organized by category:
 * - Tool Events: Tool selection and usage
 * - Object Events: Scene object manipulation
 * - View Events: Camera and viewport changes  
 * - File Events: File operations
 * - Editor Mode Events: Mode switching
 * - Asset Events: Asset management
 * - Scene Events: Scene synchronization
 */
enum class EditorEventType {
    // Tool events
    TOOL_CHANGED,        ///< A new tool has been selected in the toolbar
    TOOL_USED,           ///< The current tool has been used/activated
    
    // Object events
    OBJECT_SELECTED,     ///< An object has been selected in the scene
    OBJECT_CREATED,      ///< A new object has been created in the scene
    OBJECT_DELETED,      ///< An object has been deleted from the scene
    OBJECT_MODIFIED,     ///< An existing object has been modified
    
    // View events
    CAMERA_MOVED,        ///< The camera position has changed
    ZOOM_CHANGED,        ///< The zoom level has been adjusted
    GRID_TOGGLED,        ///< Grid visibility has been toggled
    
    // File events
    FILE_NEW,            ///< New file/project creation requested
    FILE_OPEN,           ///< File open operation requested
    FILE_SAVE,           ///< File save operation requested
    FILE_EXPORT,         ///< File export operation requested
    
    // Editor mode events
    EDITOR_MODE_CHANGED, ///< Editor mode has changed (2D/3D)
    
    // Asset events
    ASSET_SELECTED,      ///< An asset has been selected in the browser
    ASSET_LOADED,        ///< A new asset has been loaded
    
    // Scene synchronization events
    SCENE_UPDATED,       ///< The scene has been updated and UI needs refresh
    SCENE_OBJECT_ADDED,  ///< A new object has been added to the scene
    SCENE_OBJECT_REMOVED,///< An object has been removed from the scene
    SCENE_OBJECT_RENAMED ///< A scene object has been renamed
};

/**
 * @brief Event data variant type
 * 
 * A type-safe union that can hold different types of event payload data.
 * This allows events to carry different types of information depending on
 * the event type, while maintaining type safety at compile time.
 * 
 * Supported data types:
 * - int: Tool indices, object IDs, counts, etc.
 * - float: Zoom levels, distances, durations, etc.
 * - std::string: File paths, object names, messages, etc.
 * - Vector3: 3D positions, rotations, scale values
 * - Vector2: 2D positions, cursor coordinates  
 * - bool: Toggle states, flags, conditions
 */
using EventData = std::variant<
    int,                    ///< For tool indices, object IDs, etc.
    float,                  ///< For zoom levels, distances, etc.
    std::string,            ///< For file paths, object names, etc.
    Vector3,                ///< For positions, rotations, scale
    Vector2,                ///< For 2D positions, cursor coords
    bool,                    ///< For toggles, states
    BasicObject
>;

/**
 * @brief Editor event structure
 * 
 * Represents a single event in the editor event system. Each event consists of:
 * - type: The type of event (from EditorEventType enum)
 * - data: Optional payload data (using EventData variant)
 * - description: Optional human-readable description for debugging
 * 
 * Events are immutable once created and are passed by const reference
 * to event handlers to prevent modification during dispatch.
 */
struct EditorEvent {
    EditorEventType type;        ///< The type of event
    EventData data;             ///< Optional event payload data
    std::string description;    ///< Optional description for debugging
    
    /**
     * @brief Construct a new EditorEvent
     * 
     * @param t The event type
     * @param d Optional event data (default: empty)
     * @param desc Optional description (default: empty)
     */
    EditorEvent(EditorEventType t, EventData d = {}, const std::string& desc = "")
        : type(t), data(d), description(desc) {}
};

/**
 * @brief Event handler function type
 * 
 * Defines the signature for event handler functions. Event handlers receive
 * a const reference to an EditorEvent and perform the appropriate action.
 * 
 * Event handlers should be lightweight and avoid blocking operations to
 * ensure responsive event processing.
 */
using EventHandler = std::function<void(const EditorEvent&)>;

/**
 * @brief Event dispatcher class
 * 
 * The EventDispatcher is the central hub for the event-driven communication system.
 * It manages event subscriptions and dispatches events to all registered handlers.
 * 
 * Key features:
 * - Type-safe event subscription and dispatch
 * - Multiple handlers per event type support
 * - Immediate event processing (synchronous)
 * - Handler management (subscribe/unsubscribe)
 * - Thread-safe operation (when used properly)
 * 
 * Usage pattern:
 * 1. Components subscribe to events they're interested in
 * 2. Components dispatch events when actions occur
 * 3. All subscribed handlers are called immediately
 * 
 * @note Event handlers are called synchronously in the order they were registered.
 *       Handlers should avoid blocking operations to maintain responsiveness.
 */
class EventDispatcher {
public:
    /**
     * @brief Subscribe to events of a specific type
     * 
     * Registers an event handler to be called when events of the specified
     * type are dispatched. Multiple handlers can be registered for the same
     * event type and will all be called in registration order.
     * 
     * @param eventType The type of events to subscribe to
     * @param handler The handler function to call for these events
     */
    void subscribe(EditorEventType eventType, EventHandler handler);
    
    /**
     * @brief Dispatch an event to all subscribers
     * 
     * Calls all registered handlers for the event's type in the order they
     * were registered. If no handlers are registered for the event type,
     * the event is silently ignored.
     * 
     * @param event The event to dispatch
     */
    void dispatch(const EditorEvent& event);
    
    /**
     * @brief Remove all handlers for a specific event type
     * 
     * Unsubscribes all handlers that were registered for the specified
     * event type. This is useful for component cleanup.
     * 
     * @param eventType The event type to unsubscribe from
     */
    void unsubscribe(EditorEventType eventType);
    
    /**
     * @brief Clear all handlers
     * 
     * Removes all event handlers for all event types. This is typically
     * used during application shutdown or major state resets.
     */
    void clear();

private:
    /**
     * @brief Map of event types to their registered handlers
     * 
     * Each event type can have multiple handlers stored in a vector.
     * Handlers are called in the order they appear in the vector.
     */
    std::unordered_map<EditorEventType, std::vector<EventHandler>> _handlers;
};

/**
 * @brief Global event dispatcher instance
 * 
 * The global event dispatcher provides a centralized event system accessible
 * throughout the application. All components should use this instance for
 * event communication to ensure proper coordination.
 * 
 * @note This is a global singleton to simplify event system usage across
 *       different parts of the application architecture.
 */
extern EventDispatcher g_eventDispatcher;

/**
 * @brief Convenience functions for common events
 * 
 * The Events namespace provides convenient helper functions for dispatching
 * common editor events. These functions encapsulate the event creation and
 * dispatching logic, making it easier to send events from UI components.
 * 
 * Benefits:
 * - Simplified event dispatching syntax
 * - Type-safe event creation
 * - Consistent event data formatting
 * - Reduced boilerplate code
 * 
 * Usage example:
 * @code
 * // Instead of:
 * g_eventDispatcher.dispatch(EditorEvent(EditorEventType::TOOL_CHANGED, 2));
 * 
 * // Use:
 * UI::Events::toolChanged(2);
 * @endcode
 */
namespace Events {
    /**
     * @brief Dispatch a tool changed event
     * 
     * @param toolIndex Index of the newly selected tool
     */
    void toolChanged(int toolIndex);
    
    /**
     * @brief Dispatch an object selected event
     * 
     * @param objectId ID of the selected object
     */
    void objectSelected(int objectId);
    
    /**
     * @brief Dispatch an object created event
     * 
     * @param position 3D position where the object was created
     */
    void objectCreated(const Vector3& position);
    
    /**
     * @brief Dispatch an object deleted event
     * 
     * @param objectId ID of the deleted object
     */
    void objectDeleted(int objectId);
    
    /**
     * @brief Dispatch a camera move event
     * 
     * @param position New camera position
     */
    void cameraMove(const Vector3& position);
    
    /**
     * @brief Dispatch a zoom changed event
     * 
     * @param zoomLevel New zoom level value
     */
    void zoomChanged(float zoomLevel);
    
    /**
     * @brief Dispatch a grid toggled event
     * 
     * @param enabled New grid visibility state
     */
    void gridToggled(bool enabled);
    
    /**
     * @brief Dispatch a file action event
     * 
     * @param type Type of file action (FILE_NEW, FILE_OPEN, etc.)
     * @param filepath Optional file path for the operation
     */
    void fileAction(EditorEventType type, const std::string& filepath = "");
    
    /**
     * @brief Dispatch an editor mode changed event
     * 
     * @param modeIndex Index of the new editor mode (0=2D, 1=3D)
     */
    void editorModeChanged(int modeIndex);
    
    /**
     * @brief Dispatch an asset selected event
     * 
     * @param assetIndex Index of the selected asset in the browser
     */
    void assetSelected(BasicObject asset);

    // Scene synchronization events
    /**
     * @brief Dispatch a scene updated event
     * 
     * Notifies that the scene has been modified and UI should refresh.
     */
    void sceneUpdated();
    
    /**
     * @brief Dispatch a scene object added event
     * 
     * @param objectId ID of the newly added scene object
     */
    void sceneObjectAdded(int objectId);
    
    /**
     * @brief Dispatch a scene object removed event
     * 
     * @param objectId ID of the removed scene object
     */
    void sceneObjectRemoved(int objectId);
    
    /**
     * @brief Dispatch a scene object renamed event
     * 
     * @param objectId ID of the renamed scene object
     * @param newName New name for the object
     */
    void sceneObjectRenamed(int objectId, const std::string& newName);

    /**
     * @brief Dispatch a scene object renamed event
     * 
     * @param objectId ID of the renamed scene object
     * @param newName New name for the object
     */
    void addAssetRequested();
}

} // namespace UI