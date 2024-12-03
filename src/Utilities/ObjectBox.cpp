#include "ObjectBox.hpp"

using namespace Utilities;

ObjectBox2D::ObjectBox2D()
{
    _position = Vector2D(0, 0);
    _assetDimensions = Vector2D(0, 0);
    _scale = 1;
}

ObjectBox2D::ObjectBox2D(Vector2D position, Vector2D dimensions, float scale)
{
    _position = position;
    _assetDimensions = dimensions;
    _scale = scale;
}

ObjectBox2D::ObjectBox2D(const ObjectBox2D& other)
{
    _position = other.getPosition();
    _assetDimensions = other.getAssetDimensions();
    _scale = other.getScale();
}

ObjectBox2D::~ObjectBox2D()
{
}

Vector2D ObjectBox2D::getPosition() const
{
    return _position;
}

Vector2D ObjectBox2D::getDimensions() const
{
    return _assetDimensions * _scale;
}

Vector2D ObjectBox2D::getAssetDimensions() const
{
    return _assetDimensions;
}

float ObjectBox2D::getScale() const
{
    return _scale;
}

void ObjectBox2D::setPosition(Vector2D position)
{
    _position = position;
}

void ObjectBox2D::setAssetDimensions(Vector2D dimensions)
{
    _assetDimensions = dimensions;
}

void ObjectBox2D::setScale(float scale)
{
    _scale = scale;
}


ObjectBox3D::ObjectBox3D()
{
    _position = Vector3D(0, 0, 0);
    _assetDimensions = Vector3D(0, 0, 0);
    _scale = 1;
}

ObjectBox3D::ObjectBox3D(Vector3D position, Vector3D dimensions, float scale)
{
    _position = position;
    _assetDimensions = dimensions;
    _scale = scale;
}

ObjectBox3D::ObjectBox3D(const ObjectBox3D& other)
{
    _position = other.getPosition();
    _assetDimensions = other.getAssetDimensions();
    _scale = other.getScale();
}

ObjectBox3D::~ObjectBox3D()
{
}

Vector3D ObjectBox3D::getPosition() const
{
    return _position;
}

Vector3D ObjectBox3D::getDimensions() const
{
    return _assetDimensions * _scale;
}

Vector3D ObjectBox3D::getAssetDimensions() const
{
    return _assetDimensions;
}

float ObjectBox3D::getScale() const
{
    return _scale;
}


void ObjectBox3D::setPosition(Vector3D position)
{
    _position = position;
}

void ObjectBox3D::setAssetDimensions(Vector3D dimensions)
{
    _assetDimensions = dimensions;
}

void ObjectBox3D::setAssetDimensionsFromBounding(BoundingBox box) //maybe there will be a bug where the dimensions will be negative
{
    _assetDimensions = Vector3D(box.max) - Vector3D(box.min);
}

void ObjectBox3D::setScale(float scale)
{
    _scale = scale;
}

BoundingBox ObjectBox3D::convert()
{
    BoundingBox temp;
    temp.min = _position.convert();
    temp.max = (_position + (_assetDimensions * _scale)).convert();
    return temp;
}