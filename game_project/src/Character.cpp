#include "Character.hpp"

Character::Character() : BasicObject2D() {}

Character::Character(Asset2D asset, Vector2D position, int frameWidth, int frameCount, float frameSpeed)
    : BasicObject2D(asset, position), _frameWidth(frameWidth), _frameCount(frameCount), _frameSpeed(frameSpeed), _currentFrame(0), _frameTimer(0.0f) {
    _frameHeight = asset.getTexture().height; // Assuming sprite sheet is one row
}

void Character::update() {
    _frameTimer += GetFrameTime();
    if (_frameTimer >= _frameSpeed) {
        _frameTimer = 0.0f;
        _currentFrame = (_currentFrame + 1) % _frameCount;
    }
}

void Character::draw() {
    Rectangle source = {
        (float)_currentFrame * _frameWidth,
        0.0f,
        (float)_frameWidth,
        (float)_frameHeight
    };
    Vector2 position = getBox().position.convert();

    DrawTexturePro(
        getAsset().getTexture(), // inherited from Asset2D via BasicObject2D
        source,
        { position.x, position.y, _frameWidth * getBox().scale, _frameHeight * getBox().scale },
        { 0, 0 },
        0.0f,
        WHITE
    );
}
