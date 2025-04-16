#include "Character.hpp"

void Character::setPosition(const Vector2D &pos)
{
    _pos = pos;
}

void Character::setTexture(const Asset2D &asset, int frameWidth, int frameHeight, int totalFrames)
{
    _asset2D = asset;
    _frameWidth = frameWidth;
    _frameHeight = frameHeight;
    _totalFrames = totalFrames;
}

void Character::draw()
{
    if (_asset2D.isLoaded()) {
        Rectangle source = {
            _currentFrame * _frameWidth,
            0,
            (float)_frameWidth,
            (float)_frameHeight
        };
        DrawTextureRec(_asset2D.getTexture(), source, {_pos.x, _pos.y}, WHITE);
    }
}

void Character::updateAnimation()
{
    _frameCounter++;
    if (_frameCounter >= _frameSpeed) {
        _frameCounter = 0;
        _currentFrame = (_currentFrame + 1) % _totalFrames;
    }
}

void Character::setMoving(bool moving)
{
    _isMoving = moving;
    if (!moving) _currentFrame = 0;
}
