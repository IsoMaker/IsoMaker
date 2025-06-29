/*
** EPITECH PROJECT, 2025
** Iso
** File description:
** MapElement
*/

#include "MapElement.hpp"

using namespace objects;

MapElement::MapElement(Asset3D asset3D) : AEntity()
{
    _asset3D = asset3D;
    Vector3D position = Vector3D(0.0f, 0.0f, 0.0f);
    BoundingBox box = GetModelBoundingBox(_asset3D.getModel());
    Vector3D size = Vector3D(box.max.x - box.min.x, box.max.y - box.min.y, box.max.z - box.min.z);
    _box3D = ObjectBox3D(position, size);
}

MapElement::MapElement(Asset3D asset3D, Vector3D position) : AEntity()
{
    _asset3D = asset3D;
    BoundingBox box = GetModelBoundingBox(_asset3D.getModel());
    Vector3D size = Vector3D(box.max.x - box.min.x, box.max.y - box.min.y, box.max.z - box.min.z);
    _box3D = ObjectBox3D(position, size);
}

MapElement::MapElement(Asset3D asset3D, Vector3D position, Vector3D size) : AEntity()
{
    _asset3D = asset3D;
    _box3D = ObjectBox3D(position, size);
}

MapElement::MapElement(Asset3D asset3D, Vector3D position, Vector3D size, float scale) : AEntity()
{
    _asset3D = asset3D;
    _box3D = ObjectBox3D(position, size, scale);
}

MapElement::MapElement(Asset2D asset2D, Asset3D asset3D) : AEntity(asset2D)
{
    _asset3D = asset3D;
    Vector3D position = Vector3D(0.0f, 0.0f, 0.0f);
    BoundingBox box = GetModelBoundingBox(_asset3D.getModel());
    Vector3D size = Vector3D(box.max.x - box.min.x, box.max.y - box.min.y, box.max.z - box.min.z);
    _box3D = ObjectBox3D(position, size);
}

MapElement::MapElement(Asset2D asset2D, Asset3D asset3D, Vector3D position) : AEntity(asset2D)
{
    _asset3D = asset3D;
    BoundingBox box = GetModelBoundingBox(_asset3D.getModel());
    Vector3D size = Vector3D(box.max.x - box.min.x, box.max.y - box.min.y, box.max.z - box.min.z);
    _box3D = ObjectBox3D(position, size);
}

MapElement::MapElement(Asset2D asset2D, Asset3D asset3D, Vector3D position, Vector3D size) : AEntity(asset2D)
{
    _asset3D = asset3D;
    _box3D = ObjectBox3D(position, size);
}

MapElement::MapElement(Asset2D asset2D, Asset3D asset3D, Vector3D position, Vector3D size, float scale) : AEntity(asset2D)
{
    _asset3D = asset3D;
    _box3D = ObjectBox3D(position, size, scale);
}

MapElement::~MapElement()
{
}

Asset3D MapElement::getAsset3D() const
{
    return _asset3D;
}

void MapElement::setAsset3D(Asset3D asset3D)
{
    _asset3D = asset3D;
}

void MapElement::draw()
{
    DrawModel(_asset3D.getModel(), _box3D.getPosition().convert(), _box3D.getScale(), WHITE);
}
