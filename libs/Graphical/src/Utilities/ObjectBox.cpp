/*
** EPITECH PROJECT, 2025
** Iso
** File description:
** ObjectBox
*/

#include "ObjectBox.hpp"

using namespace Utilities;

void ObjectBox2D::setPosX(float x)
{
    _position.x = x;
}

void ObjectBox2D::setPosY(float y)
{
    _position.y = y;
}

Rectangle ObjectBox2D::getRectangle()
{
    return { _position.x, _position.y, _size.x, _size.y };
}

bool ObjectBox2D::isInBox(Vector2D pos)
{
    if (pos > _position && pos < (_position + (_size * _scale)))
        return true;
    return false;
}

void ObjectBox3D::setPosX(float x)
{
    _position.x = x;
}

void ObjectBox3D::setPosY(float y)
{
    _position.y = y;
}

void ObjectBox3D::setPosZ(float z)
{
    _position.z = z;
}

void ObjectBox3D::setSizeFromBounding(BoundingBox box)
{
    _size = Vector3D(box.max) - Vector3D(box.min);
}

BoundingBox ObjectBox3D::convert()
{
    Vector3 min = _position.convert();
    Vector3 max = ((_position + _size) * _scale).convert();
    BoundingBox box = { min, max };
    return box;
}
