#pragma once
/**
 * @file IAsset.hpp
 * @brief Base interface for all assets in the IsoMaker graphical library
 * @author IsoMaker Team
 * @version 0.1
 */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include "raylib.h"

/**
 * @brief Base interface for all assets
 * 
 * The IAsset interface defines the common contract for all assets in the IsoMaker
 * system, including 2D and 3D assets. It provides a unified API for asset metadata
 * management, file handling, and tag-based organization.
 * 
 * Key responsibilities:
 * - File name and display name management
 * - Tag-based categorization and searching
 * - Asset loading and file operations
 * - Metadata management
 * 
 * The tag system allows for flexible asset organization and filtering:
 * - Tags can be used for categories (e.g., "environment", "character")
 * - Tags can indicate properties (e.g., "animated", "transparent")
 * - Tags enable powerful search and filtering capabilities
 * 
 * @see Asset2D For 2D asset implementation
 * @see Asset3D For 3D asset implementation
 */
class IAsset
{
    public:
        /**
         * @brief Get the asset's file name
         * 
         * Returns the file name (including path) where this asset is stored.
         * 
         * @return std::string The asset's file name
         */
        virtual std::string getFileName() = 0;
        
        /**
         * @brief Get the asset's display name
         * 
         * Returns the human-readable display name for this asset,
         * which may be different from the file name.
         * 
         * @return std::string The asset's display name
         */
        virtual std::string getDisplayName() = 0;

        /**
         * @brief Get all tags associated with this asset
         * 
         * Returns a list of all tags that have been assigned to this asset.
         * 
         * @return std::vector<std::string> List of all asset tags
         */
        virtual std::vector<std::string> getTags() = 0;

        /**
         * @brief Set the asset's file name
         * 
         * Updates the file name (including path) for this asset.
         * 
         * @param fileName The new file name to set
         */
        virtual void setFileName(std::string fileName) = 0;
        
        /**
         * @brief Set the asset's display name
         * 
         * Updates the human-readable display name for this asset.
         * 
         * @param displayName The new display name to set
         */
        virtual void setDisplayName(std::string displayName) = 0;

        /**
         * @brief Replace all tags with a new set
         * 
         * Removes all existing tags and replaces them with the provided list.
         * 
         * @param tags The new list of tags to set
         */
        virtual void setTags(std::vector<std::string> tags) = 0;
        
        /**
         * @brief Add multiple tags to the asset
         * 
         * Adds all tags from the provided list to the asset's existing tags.
         * 
         * @param tags List of tags to add
         */
        virtual void addTags(std::vector<std::string> tags) = 0;
        
        /**
         * @brief Add a single tag to the asset
         * 
         * Adds the specified tag to the asset's list of tags.
         * 
         * @param tag The tag to add
         */
        virtual void addTag(std::string tag) = 0;
        
        /**
         * @brief Remove multiple tags from the asset
         * 
         * Removes all specified tags from the asset's tag list.
         * 
         * @param tags List of tags to remove
         */
        virtual void removeTags(std::vector<std::string> tags) = 0;
        
        /**
         * @brief Remove a single tag from the asset
         * 
         * Removes the specified tag from the asset's tag list.
         * 
         * @param tag The tag to remove
         */
        virtual void removeTag(std::string tag) = 0;
        
        /**
         * @brief Check if the asset has all specified tags
         * 
         * Returns true only if the asset has ALL of the specified tags.
         * 
         * @param tags List of tags to search for
         * @return true if all tags are found, false otherwise
         */
        virtual bool findAllTags(std::vector<std::string> tags) = 0;
        
        /**
         * @brief Check if the asset has any of the specified tags
         * 
         * Returns true if the asset has ANY of the specified tags.
         * 
         * @param tags List of tags to search for
         * @return true if any tag is found, false otherwise
         */
        virtual bool findAnyTags(std::vector<std::string> tags) = 0;
        
        /**
         * @brief Check if the asset has a specific tag
         * 
         * Returns true if the asset has the specified tag.
         * 
         * @param tag The tag to search for
         * @return true if the tag is found, false otherwise
         */
        virtual bool findTag(std::string tag) = 0;

        /**
         * @brief Load the asset file into memory
         * 
         * Loads the asset data from the file system into memory, making it
         * ready for use in rendering or other operations.
         */
        virtual void loadFile() = 0;
};
