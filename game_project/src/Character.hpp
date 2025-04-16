#pragma once

#include "Entities/BasicObject2D.hpp"

class Character {
    public:

        void setPosition(const Vector2D &pos);
        void setTexture(const Asset2D &asset, int frameWidth, int frameHeight, int totalFrames);
        void draw();
        void updateAnimation();
        void setMoving(bool moving);
        Asset2D getAsset() { return _asset2D; };


    
    private:
        Vector2D _pos;
        Asset2D _asset2D;
    
        int _frameWidth = 32;
        int _frameHeight = 32;
        int _totalFrames = 1;
        int _currentFrame = 0;
        int _frameCounter = 0;
        int _frameSpeed = 8;
    
        bool _isMoving = false;
};
