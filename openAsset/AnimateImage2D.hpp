#pragma once
#include "raylib.h"
#include <string>
#include "Entities/BasicObject.hpp"

class AnimatedSprite {
public:
    AnimatedSprite() = default;

    bool load(std::string path, int frameWidth, int frameHeight, int totalFrames);

    void updateAnimation();

    void draw(Vector2 position, float scale);

    void unload();

    ~AnimatedSprite() {
        unload();
    }

    int getFrameWidth() { return _frameWidth; };
    int getFrameHeight() { return _frameHeight; };
    int getTotalFrames() { return _totalFrames; };

    void setFrameWidth(int frameWidth) { _frameWidth = frameWidth; };
    void setFrameHeight(int frameHeight) { _frameHeight = frameHeight; };
    void setTotalFrames(int totalFrames) { _totalFrames = totalFrames; };

private:
    Texture2D _texture;
    ObjectBox3D _objectBox;
    Vector3D _position;

    int _frameWidth = 0;
    int _frameHeight = 0;
    int _totalFrames = 1;
    int _currentFrame = 0;
    int _frameCounter = 0;
    int _frameSpeed = 8;

    bool _isMoving = false;
};
