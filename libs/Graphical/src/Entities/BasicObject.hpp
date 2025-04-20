#pragma once

#include <vector>
#include <raylib.h>

#include "IObject3D.hpp"

#define SCREENHEIGHT 1200
#define SCREENWIDTH 1600

using namespace Utilities;

void DrawCubeTexture(Texture2D texture, Vector3 position, float width, float height, float length, Color color);

enum AssetType {
    ASSET3D,
    ASSET2D,
};

class BasicObject : public IObject3D {
    public:
        BasicObject() {moveTo(Vector3D(0, 0, 0));};
        BasicObject(Asset3D, Vector3D);
        BasicObject(Asset2D, Vector3D, Vector3D dimension3D);
        ~BasicObject() {};

        void setAsset(Asset3D);
        void setAsset(Asset2D, Vector3D dimension3D);
        void setTexture(Asset2D, int frameWidth, int frameHeight, int totalFrames);
        void setMoving(bool moving) { _isMoving = moving; };

        void draw();
        void updateAnimation();

        void move(Vector3D positionModifier) {_objectBox.position = _objectBox.position + positionModifier;};
        void moveTo(Vector3D newPosition) {_objectBox.position = newPosition;};
        void resizePercent(float percentage) {_objectBox.scale = percentage;};
        void resizeTo(float height);

        ObjectBox3D &getBox() override {return _objectBox;};
        AssetType getAssetType() { return _assetType; };
        Vector3D getPosition();
        Asset3D getAsset3D();
        Asset2D getAsset2D();

    private:
        ObjectBox3D _objectBox;
        Asset3D _asset3D;
        Asset2D _asset2D;
        AssetType _assetType;
        Vector3D _position;

        int _frameWidth = 0;
        int _frameHeight = 0;
        int _totalFrames = 1;
        int _currentFrame = 0;
        int _frameCounter = 0;
        int _frameSpeed = 8;

        bool _isMoving = false;
};
