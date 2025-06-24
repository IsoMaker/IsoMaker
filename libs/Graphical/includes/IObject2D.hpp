/**
 * @file IObject2D.hpp
 * @brief Interface for 2D objects in the IsoMaker graphical library
 * @author IsoMaker Team
 * @version 0.1
 */

#pragma once

#include "../src/Utilities/Vector.hpp"
#include "../src/Assets/Asset2D.hpp"
#include "../src/Utilities/ObjectBox.hpp"

using namespace Utilities;

/**
 * @brief Interface for 2D objects
 * 
 * The IObject2D interface defines the contract for all 2D objects in the IsoMaker
 * graphical system. It provides a consistent API for 2D asset management, rendering,
 * transformation, and spatial queries.
 * 
 * Key responsibilities:
 * - 2D asset assignment and management
 * - 2D rendering capabilities
 * - 2D spatial transformations (move, resize)
 * - 2D bounding box queries for collision detection
 * 
 * This interface is primarily used for sprites, UI elements, and 2D game objects
 * within the pixel art editor and other 2D components of the system.
 * 
 * @see BasicObject2D For the main implementation of this interface
 * @see Asset2D For 2D asset management
 * @see ObjectBox2D For 2D spatial queries
 */
class IObject2D
{
    public:
        /**
         * @brief Set a 2D asset for this object
         * 
         * Assigns a 2D asset (sprite, texture, image) to this object for rendering.
         * 
         * @param asset The 2D asset to assign to this object
         */
        virtual void setAsset(Asset2D asset) = 0;
        
        /**
         * @brief Render the 2D object
         * 
         * Draws the object using its current 2D asset, position, and transformation.
         * This method is called during the 2D rendering phase.
         */
        virtual void draw() = 0;
        
        /**
         * @brief Move the object by a relative 2D offset
         * 
         * Translates the object's position by the specified 2D vector offset.
         * 
         * @param offset The 2D vector offset to move by
         */
        virtual void move(Vector2D offset) = 0;
        
        /**
         * @brief Move the object to an absolute 2D position
         * 
         * Sets the object's position to the specified absolute 2D coordinates.
         * 
         * @param position The absolute 2D position to move to
         */
        virtual void moveTo(Vector2D position) = 0;
        
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
         * @brief Get the object's 2D bounding box
         * 
         * Returns the object's 2D bounding box for collision detection,
         * spatial queries, and selection operations in 2D space.
         * 
         * @return ObjectBox2D The object's 2D bounding box
         */
        virtual ObjectBox2D getBox() = 0;
};
