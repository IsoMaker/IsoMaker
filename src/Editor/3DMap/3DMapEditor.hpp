/**
 * @file 3DMapEditor.hpp
 * @brief 3D map editor component for the IsoMaker game engine
 * @author IsoMaker Team
 * @version 0.1
 */

#pragma once

#include <vector>
#include <limits>
#include "raylib.h"
#include "rlgl.h"

#include <fstream>

#include "Render/Window.hpp"
#include "Render/Camera.hpp"
#include "Grid.hpp"

#include "Input/MouseKeyboard.hpp"

#include "Entities/MapElement.hpp"
#include "Entities/Character.hpp"

#include "../../UI/EditorEvents.hpp"
#include "../../UI/SceneObject.hpp"

#include "../../Utilities/LoadedAssets.hpp"

using namespace Utilities;
using namespace objects;

/**
 * @brief Asset type enumeration
 * 
 * Defines the types of assets that can be placed in the 3D map editor.
 */
namespace asset {
    /**
     * @brief Asset type enumeration
     */
    enum class Type
    {
       PLAYER, ///< Player character asset
       CUBE,   ///< Cube/block asset
    };
}

/**
 * @brief 3D Map Editor class
 * 
 * The MapEditor class provides comprehensive 3D scene editing capabilities for the IsoMaker
 * game engine. It handles object placement, manipulation, and scene management with full
 * integration into the event-driven UI system.
 * 
 * Key features:
 * - 3D object placement and manipulation (cubes, players)
 * - Grid-aligned positioning system
 * - Real-time scene preview with visual feedback
 * - File I/O for map saving and loading
 * - Event-driven communication with UI components
 * - Scene provider interface for UI integration
 * - Game compilation for runtime execution
 * 
 * The editor supports multiple tools:
 * - SELECT: Object selection and manipulation
 * - CUBE: 3D object placement
 * - HAND: Camera navigation
 * 
 * @see UI::ISceneProvider For scene data interface
 * @see UI::EditorEvents For event communication
 * @see MapGrid For grid system
 */
class MapEditor : public UI::ISceneProvider {
    public:

        /**
         * @brief Construct a new MapEditor object
         */
        MapEditor();

        /**
         * @brief Destroy the MapEditor object
         * 
         * Cleanup is handled automatically by member destructors.
         */
        ~MapEditor();

                /**
         * @brief Initialize te map editor's main parameters
         * 
         * Initializes the 3D map editor with camera and window references.
         * Sets up the grid system and default editor state.
         * 
         * @param camera Reference to the rendering camera
         * @param window Reference to the application window
         */
        void init(std::shared_ptr<Render::Window> window, std::shared_ptr<Render::Camera> camera);

        /**
         * @brief Initialize the grid system
         * 
         * Sets up the 3D grid used for object alignment and visual reference.
         */
        void initGrid();

        /**
         * @brief Update the editor state
         * 
         * Updates the editor based on input, handles object placement/selection,
         * and processes tool-specific functionality. Called once per frame.
         * 
         * @param inputHandler Reference to the input handler
         */
        void update(input::IHandlerBase &inputHandler);

        /**
         * @brief Draw 2D UI elements
         * 
         * Renders 2D overlay elements like cursor information and HUD elements.
         */
        void draw2DElements();

        /**
         * @brief Draw 3D scene elements
         * 
         * Renders all 3D objects, the grid, and preview objects in the scene.
         */
        void draw3DElements();

                /**
         * @brief Draw all map editor elements
         * 
         * Calls other draw methods of map editor to render all 3D and 2D elements
         * 
         * @param mainViewArea Rectangle representing view area of map editor
         */
        void draw(Rectangle mainViewArea);

        /**
         * @brief Change the current cube type for placement
         * 
         * Sets the 3D asset to be used for new cube placements.
         * 
         * @param asset The 3D asset to use for new cubes
         */
        void changeCubeType(Asset3D asset);

        /**
         * @brief Add a cube at the specified position
         * 
         * Places a new cube object at the given 3D position.
         * 
         * @param position The 3D position where to place the cube
         */
        void addCube(Vector3D position);

        /**
         * @brief Remove a cube object
         * 
         * Removes the cube object pointed to by the iterator.
         * 
         * @param cubeIterator Iterator pointing to the cube to remove
         */
        void removeCube(std::vector<std::shared_ptr<MapElement>>::iterator cubeIterator);

        /**
         * @brief Change the current sprite type for 2D objects
         * 
         * Sets the 2D asset to be used for new sprite placements.
         * 
         * @param newAsset The 2D asset to use for new sprites
         */
        void changeTextureType(Asset2D newAsset);

        /**
         * @brief Change the current sprite type for 2D objects
         * 
         * Sets the 2D asset to be used for new sprite placements.
         * 
         * @param newAsset The 2D asset to use for new sprites
         */
        void changeSpriteType(Asset2D newAsset);
        
        /**
         * @brief Add a player at the specified 3D position
         * 
         * Places a player object at the given 3D position.
         * 
         * @param position The 3D position where to place the player
         */
        void addPlayer(Vector3D position);
        
        /**
         * @brief Remove a player object
         * 
         * Removes the player object pointed to by the iterator.
         * 
         * @param toRemove Iterator pointing to the player to remove
         */
        void removePlayer(std::vector<std::shared_ptr<Character>>::iterator toRemove);

        /**
         * @brief Save the map to a binary file
         * 
         * Serializes the current map state to a binary file format.
         * 
         * @param filename Path to the output file
         */
        void saveMap(const std::string& filename);
        
        /**
         * @brief Load a map from a binary file
         * 
         * Deserializes a map from a binary file format.
         * 
         * @param filename Path to the input file
         */
        void loadMap(const std::string& filename);

        /**
         * @brief Compile the map for game runtime
         * 
         * Generates game-ready assets and data for the specified game project.
         * 
         * @param gameProjectName Name of the target game project
         */
        void gameCompilation(const std::string& gameProjectName);
        
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
        
        /**
         * @brief Handle asset selection events
         * 
         * Responds to asset selection from the assets browser.
         * 
         * @param assetIndex Index of the selected asset
         * @param loadedAssets2D Vector of asset2D
         */
        void handleAssetSelected(std::shared_ptr<AAsset> asset);

        /**
         * @brief Handle asset selection events
         * 
         * Responds to asset selection from the assets browser.
         * 
         * @param assetIndex Index of the selected asset
         */
        void handleAssetLoaded();
        
        // State queries for UI
        /**
         * @brief Get the total number of objects in the scene
         * 
         * @return int Total count of 3D and 2D objects
         */
        int getObjectCount() const;
        
        /**
         * @brief Get the ID of the currently selected object
         * 
         * @return int ID of selected object, or -1 if none selected
         */
        int getSelectedObjectId() const override;
        
        /**
         * @brief Get the name of the currently selected object
         * 
         * @return std::string Name of the selected object, or empty if none
         */
        std::string getSelectedObjectName() const;
        
        /**
         * @brief Get the current camera position
         * 
         * @return Vector3 Current camera position in 3D space
         */
        Vector3 getCameraPosition() const;
        
        /**
         * @brief Check if the grid is visible
         * 
         * @return true if grid is visible, false otherwise
         */
        bool isGridVisible() const;
        
        // ISceneProvider interface implementation
        /**
         * @brief Get all scene objects for UI display
         * 
         * @return std::vector<UI::SceneObjectInfo> List of scene objects
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
        
        // Scene management
        /**
         * @brief Update the scene objects list
         * 
         * Refreshes internal scene object tracking for UI synchronization.
         */
        void updateSceneObjects();
        
        /**
         * @brief Notify UI of scene changes
         * 
         * Dispatches scene update events to keep UI synchronized.
         */
        void notifySceneChanged();

        void setLoader(std::shared_ptr<AssetLoader> loader);

    protected:
    private:
        // Internal helper methods
        /**
         * @brief Find position from ray collision
         * 
         * Determines object placement position from a ray collision result.
         * 
         * @param hit Ray collision data
         */
        void findPositionFromHit(RayCollision &hit);
        
        /**
         * @brief Find position from grid intersection
         * 
         * Calculates position where ray intersects with the grid plane.
         * 
         * @param ray The ray to test against the grid
         */
        void findPositionFromGrid(Ray &ray);
        
        /**
         * @brief Update cursor information display
         * 
         * Updates UI elements showing cursor position and camera information.
         * 
         * @param cursorPos 2D cursor position
         * @param cameraPos 3D camera position
         */
        void updateCursor();

        // Scene objects
        std::vector<std::shared_ptr<MapElement>> _objects3D; ///< All 3D objects in the scene
        std::vector<std::shared_ptr<Character>> _objects2D;  ///< All 2D objects in the scene

        // Current assets
        Asset2D _currentTextureType;                         ///< Currently selected texture for 3D asset placement;
        Asset3D _currentCubeType;                            ///< Currently selected 3D asset for placement
        Asset2D _currentSpriteType;                          ///< Currently selected 2D asset for placement
        bool _blocSelect;

        // Assets Loaded
        std::shared_ptr<AssetLoader> _loader;
        std::vector<Asset3D> _objects3DLoaded;            ///< All 3D objects loaded
        std::vector<Asset2D> _objects2DLoaded;             ///< All 2D objects loaded

        // Core references
        std::shared_ptr<Render::Window> _window;             ///< Reference to the application window
        std::shared_ptr<Render::Camera> _camera;             ///< Reference to the 3D camera
        map::MapGrid _grid;                                  

        // Editor state
        bool _placePlayer;                                   ///< Flag for player placement mode
        bool _drawWireframe = false;                         ///< Wireframe rendering mode
        float _cubeHeight;                                   ///< Height for cube placement

        // Interactive elements
        Vector2D _cursorPosition;
        Vector3D _alignedPosition;                           ///< Current grid-aligned cursor position
        std::optional<std::vector<std::shared_ptr<MapElement>>::iterator> _closestObject; ///< Closest object to cursor
        std::optional<std::vector<std::shared_ptr<Character>>::iterator> _closestSprite; ///< Closest object to cursor
        
        // Current tool and selection state
        int _currentTool = 0;                                ///< Current tool index (default: SELECT)
        int _selectedObjectId = -1;                          ///< ID of currently selected object
        bool _gridVisible = true;                            ///< Grid visibility state
};
