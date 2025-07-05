/*
** EPITECH PROJECT, 2025
** Iso
** File description:
** Character
*/

#include "Character.hpp"

#include <raymath.h>
#include <algorithm>

using namespace objects;

Character::Character(Asset2D asset) : AEntity(asset)
{
}

Character::Character(Asset2D asset, Vector3D position) : AEntity(asset, position)
{
}

Character::Character(Asset2D asset, Vector3D position, Vector2D framePosition, Vector2D frameSize) : AEntity(asset, position)
{
    _box2D.setPosition(framePosition);
    _box2D.setSize(frameSize);
}

Character::~Character()
{
}

bool Character::isMoving()
{
    return _isMoving;
}

void Character::setMoving(bool moving)
{
    _isMoving = moving;
}

void Character::updateAnimation()
{
    if (_isMoving) {
        _frameCounter += 1;
        if (_frameCounter >= _frameSpeed) {
            _frameCounter = 0;
            _currentFrame = (_currentFrame + 1) % _totalFrames;
        }
        _box2D.setPosX(_currentFrame * _box2D.getSize().x);
    } else {
        _frameCounter = 0;
        _currentFrame = 1;
        _box2D.setPosX(0);
    }
}

void Character::draw(Vector2D tileSize)
{
    Rectangle source = _box2D.getRectangle();
    Vector3D pos = _box3D.getPosition();
    const float tileWidth = 64.0f;
    const float tileHeight = 32.0f;
    float isoX = (pos.x - pos.z) * tileWidth / 2.0f;
    float isoY = (pos.x + pos.z) * tileHeight / 2.0f - pos.y * tileHeight;
    Vector2 position = {
        (isoX + SCREENWIDTH / 2) - source.width,
        ((isoY + SCREENHEIGHT / 2) - source.height) - 48
    };

    DrawTextureRec(_asset2D.getTexture(), source, position, WHITE);
    // Vector3D forward = Vector3D(camera->getTarget() - camera->getPosition());
    // Vector3D up = Vector3D(0.0f, 1.0f, 0.0f);
    // up = forward * (up * forward);
    // up = Vector3D(Vector3Normalize(up.convert()));
    // Texture2D texture = _asset2D.getTexture();
    // Rectangle source = _box2D.getRectangle();
    // Vector3D position = {0, 3, 0};
    // Vector2D size = _box2D.getSize();
    // Vector2D origin = size * _box2D.getScale();

    // DrawBillboardPro(camera->getRaylibCam(), texture, source, position.convert(), up.convert(), size.convert(), origin.convert(), 0.0f, WHITE);
}

void Character::draw(Vector3D tmp)
{
    Rectangle source = _box2D.getRectangle();
    Vector3D pos = _box3D.getPosition();
    Vector2 position = {
        150 + (source.width * 2),
        150 + (source.height * 2)
    };

    DrawTextureRec(_asset2D.getTexture(), source, position, WHITE);
}
