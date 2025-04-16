#pragma once

#include "Entities/BasicObject2D.hpp"

class Character : public BasicObject2D {
public:
    Character();
    Character(Asset2D asset, Vector2D position, int frameWidth, int frameCount, float frameSpeed);

    void update();  // to advance animation
    void draw();    // override draw to only draw one frame

private:
    int _frameWidth;
    int _frameHeight;
    int _frameCount;
    int _currentFrame;
    float _frameSpeed;
    float _frameTimer;
};
