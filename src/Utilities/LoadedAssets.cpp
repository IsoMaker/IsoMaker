#include "LoadedAssets.hpp"

void AssetLoader::updateAssets(const std::string& directoryPath)
{
    if (!std::filesystem::exists(directoryPath) || !std::filesystem::is_directory(directoryPath)) {
        std::cerr << "Invalid directory: " << directoryPath << std::endl;
        return;
    }

    std::string line;
    std::string type;

    for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
        if (entry.path().extension() == ".txt") {
            std::string filePath = entry.path().string();

            std::ifstream inFile(filePath);

            for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
                if (entry.path().extension() == ".txt") {
                    std::string filePath = entry.path().string();

                    if (std::find(_loadedFiles.begin(), _loadedFiles.end(), filePath) != _loadedFiles.end()) {
                        continue;
                    }

                    std::ifstream inFile(filePath);

                    if (!inFile.is_open()) {
                        std::cerr << "Failed to open file: " << filePath << std::endl;
                        continue;
                    }

                    while (std::getline(inFile, line)) {
                        if (line.find("Type:") == 0) {
                            type = line.substr(6);

                            if (type == "2D") {
                                _loadedFiles.push_back(filePath);
                                _loadedAssets2D.push_back(loadAssetFile2D(inFile));
                            } else if (type == "3D") {
                                _loadedFiles.push_back(filePath);
                                _loadedAssets3D.push_back(loadAssetFile3D(inFile));
                            }
                        }
                    }
                }
            }
        }
    }
}

Asset2D AssetLoader::loadAssetFile2D(std::ifstream& inFile)
{
    std::string line;
    std::string name;
    std::string file;
    std::string sizeOrScaledSize;
    float scale = 1.0f;
    int frames = 0;
    int width = 0;
    int height = 0;

    while (std::getline(inFile, line)) {
        if (line.find("Name:") == 0) {
            name = line.substr(6);
        } else if (line.find("File:") == 0) {
            file = line.substr(6);
        } else if (line.find("Scale:") == 0) {
            scale = std::stof(line.substr(7));
        } else if (line.find("Scaled Size:") == 0 || line.find("Size:") == 0) {
            sizeOrScaledSize = line.substr(line.find(":") + 2);
        } else if (line.find("Frames:") == 0) {
            frames = std::stoi(line.substr(8));
        }
    }
    inFile.close();

    Asset2D asset;
    asset.setFileName(file);
    asset.setDisplayName(name);
    asset.setScale(scale);
    std::stringstream ss(sizeOrScaledSize);
    char ignoreChar;
    ss >> width >> ignoreChar >> height;
    asset.setWidth(width);
    asset.setHeight(height);
    asset.loadFile();
    std::cout << sizeOrScaledSize << std::endl;
    return asset;
}

Asset3D AssetLoader::loadAssetFile3D(std::ifstream& inFile)
{
    std::string line;
    std::string name;
    std::string file;
    std::string sizeOrScaledSize;
    float scale = 1.0f;
    int frames = 0;
    int width = 0;
    int height = 0;

    while (std::getline(inFile, line)) {
        if (line.find("Name:") == 0) {
            name = line.substr(6);
        } else if (line.find("File:") == 0) {
            file = line.substr(6);
        } else if (line.find("Scale:") == 0) {
            scale = std::stof(line.substr(7));
        } else if (line.find("Scaled Size:") == 0 || line.find("Size:") == 0) {
            sizeOrScaledSize = line.substr(line.find(":") + 2);
        } else if (line.find("Frames:") == 0) {
            frames = std::stoi(line.substr(8));
        }
    }
    inFile.close();

    Asset3D asset;
    asset.setFileName(file);
    asset.setDisplayName(name);
    asset.setScale(scale);
    asset.loadFile();
    return asset;
}

const std::vector<Asset2D>& AssetLoader::getLoaded2DAssets() const {
    return _loadedAssets2D;
}

const std::vector<Asset3D>& AssetLoader::getLoaded3DAssets() const {
    return _loadedAssets3D;
}
