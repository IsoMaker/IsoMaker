#pragma once
#include "raylib.h"
#include <string>
#include "AnimateImage2D.hpp"

bool AnimatedSprite::load(std::string path, int frameWidth, int frameHeight, int totalFrames) {
    if (_texture.id != 0) {
        unload();
    }

    _texture = LoadTexture(path.c_str());

    _frameWidth = frameWidth;
    _frameHeight = frameHeight;
    _totalFrames = totalFrames;

    _currentFrame = 0;
    _frameCounter = 0;
    return true;
}

void AnimatedSprite::updateAnimation() {
    _frameCounter++;
    if (_frameCounter >= _frameSpeed) {
        _frameCounter = 0;
        _currentFrame = (_currentFrame + 1) % _totalFrames;
    }
}

void AnimatedSprite::draw(Vector2 position, float scale)
{
    if (_texture.id == 0) return;

    Rectangle source = {
        (float)(_currentFrame * _frameWidth), 0,
        (float)_frameWidth, (float)_frameHeight
    };

    Rectangle dest = {
        position.x, position.y,
        _frameWidth * scale, _frameHeight * scale
    };

    DrawTextureRec(_texture, source, position, WHITE);
}

void AnimatedSprite::unload()
{
    if (_texture.id != 0) {
        UnloadTexture(_texture);
    }
}
