#pragma once

#include "Entities/BasicObject.hpp"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <optional>

class AssetLoader {
    public:
        void updateAssets(const std::string& directoryPath);
        std::optional<BasicObject> loadAssetFile(const std::string& filePath);

        const std::vector<Asset2D>& getLoaded2DAssets() const;
        const std::vector<Asset3D>& getLoaded3DAssets() const;

    private:
        std::vector<std::string> _loadedFiles;
        std::vector<Asset2D> _loadedAssets2D;
        std::vector<Asset3D> _loadedAssets3D;
};
