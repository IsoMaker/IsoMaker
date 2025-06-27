/*
** EPITECH PROJECT, 2025
** Iso
** File description:
** Character
*/

#include "Character.hpp"

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
    return _moving;
}

void Character::setMoving(bool moving)
{
    _moving = moving;
}

void Character::updateAnimation() {
    _frameCounter++;
    if (_frameCounter >= _frameSpeed) {
        _frameCounter = 0;
        std::clamp(_frameCounter + 1, 0, _totalFrames);
    }
}

void Character::draw(Render::Camera &camera)
{
    // DrawTextureRec(_asset2D.getTexture(), source, position, WHITE);
    DrawBillboardRec(camera.getRaylibCam(), _asset2D.getTexture(), _box2D.getRectangle(), _box3D.getPosition().convert(), _box2D.getSize().convert(), WHITE);
}
