#pragma once

#include <string>
#include <memory>
#include "raylib.h"

namespace UI {

enum class SceneObjectType {
    CUBE_3D,
    SPRITE_2D,
    CAMERA,
    LIGHT,
    UNKNOWN
};

struct SceneObjectInfo {
    int id;
    std::string name;
    SceneObjectType type;
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
    bool isSelected;
    bool isVisible;
    
    SceneObjectInfo() 
        : id(-1), name("Unknown"), type(SceneObjectType::UNKNOWN), 
          position({0, 0, 0}), rotation({0, 0, 0}), scale({1, 1, 1}),
          isSelected(false), isVisible(true) {}
    
    SceneObjectInfo(int objId, const std::string& objName, SceneObjectType objType, 
                   Vector3 pos = {0, 0, 0}, Vector3 rot = {0, 0, 0}, Vector3 scl = {1, 1, 1})
        : id(objId), name(objName), type(objType), position(pos), rotation(rot), scale(scl),
          isSelected(false), isVisible(true) {}
          
    std::string getDisplayName() const {
        std::string typePrefix;
        switch (type) {
            case SceneObjectType::CUBE_3D: typePrefix = "Cube"; break;
            case SceneObjectType::SPRITE_2D: typePrefix = "Sprite"; break;
            case SceneObjectType::CAMERA: typePrefix = "Camera"; break;
            case SceneObjectType::LIGHT: typePrefix = "Light"; break;
            default: typePrefix = "Object"; break;
        }
        return typePrefix + "_" + std::to_string(id);
    }
};

// Scene manager interface for editors to implement
class ISceneProvider {
public:
    virtual ~ISceneProvider() = default;
    virtual std::vector<SceneObjectInfo> getSceneObjects() const = 0;
    virtual SceneObjectInfo getObjectInfo(int objectId) const = 0;
    virtual bool selectObject(int objectId) = 0;
    virtual bool deleteObject(int objectId) = 0;
    virtual int getSelectedObjectId() const = 0;
};

} // namespace UI