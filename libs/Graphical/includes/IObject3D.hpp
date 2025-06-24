/**
 * @file IObject3D.hpp
 * @brief Interface for 3D objects in the IsoMaker graphical library
 * @author IsoMaker Team
 * @version 0.1
 */

#pragma once

#include "../src/Utilities/Vector.hpp"
#include "../src/Assets/Asset2D.hpp"
#include "../src/Assets/Asset3D.hpp"
#include "../src/Utilities/ObjectBox.hpp"

using namespace Utilities;

/**
 * @brief Interface for 3D objects
 * 
 * The IObject3D interface defines the contract for all 3D objects in the IsoMaker
 * graphical system. It provides a consistent API for asset management, rendering,
 * transformation, and spatial queries.
 * 
 * Key responsibilities:
 * - Asset assignment and management (both 3D and 2D assets)
 * - 3D rendering capabilities
 * - Spatial transformations (move, resize)
 * - Bounding box queries for collision detection
 * 
 * All 3D objects in the engine must implement this interface to ensure
 * compatibility with the rendering and interaction systems.
 * 
 * @see BasicObject For the main implementation of this interface
 * @see Asset3D For 3D asset management
 * @see ObjectBox3D For spatial queries
 */
class IObject3D
{
    public:
        /**
         * @brief Set a 3D asset for this object
         * 
         * Assigns a 3D asset (model, texture, etc.) to this object for rendering.
         * 
         * @param asset The 3D asset to assign to this object
         */
        virtual void setAsset(Asset3D asset) = 0;
        
        /**
         * @brief Set a 2D asset with 3D dimensions
         * 
         * Assigns a 2D asset (sprite, texture) to this object but treats it as
         * a 3D object with the specified dimensions.
         * 
         * @param asset The 2D asset to assign
         * @param dimension3D The 3D dimensions for the 2D asset
         */
        virtual void setAsset(Asset2D asset, Vector3D dimension3D) = 0;
        
        /**
         * @brief Render the object
         * 
         * Draws the object using its current asset, position, and transformation.
         * This method is called during the rendering phase.
         */
        virtual void draw() = 0;
        
        /**
         * @brief Move the object by a relative offset
         * 
         * Translates the object's position by the specified vector offset.
         * 
         * @param offset The 3D vector offset to move by
         */
        virtual void move(Vector3D offset) = 0;
        
        /**
         * @brief Move the object to an absolute position
         * 
         * Sets the object's position to the specified absolute coordinates.
         * 
         * @param position The absolute 3D position to move to
         */
        virtual void moveTo(Vector3D position) = 0;
        
        /**
         * @brief Resize the object by a percentage
         * 
         * Scales the object's size by the specified percentage factor.
         * A value of 1.0 means no change, 0.5 means half size, 2.0 means double size.
         * 
         * @param scale The scaling factor as a percentage
         */
        virtual void resizePercent(float scale) = 0;
        
        /**
         * @brief Get the object's bounding box
         * 
         * Returns a reference to the object's 3D bounding box for collision
         * detection, spatial queries, and selection operations.
         * 
         * @return ObjectBox3D& Reference to the object's bounding box
         */
        virtual ObjectBox3D &getBox() = 0;
};
