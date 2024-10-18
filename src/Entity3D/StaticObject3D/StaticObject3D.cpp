#include "StaticObject3D.hpp"

using namespace Entity3D;
using namespace Utilities;

StaticObject3D::StaticObject3D()
{

}

StaticObject3D::StaticObject3D(Asset3D asset, Vector3D position)
{
    setAsset(asset);
    moveTo(position);
}

StaticObject3D::~StaticObject3D()
{

}

void StaticObject3D::setAsset(Asset3D newAsset)
{
    _asset = newAsset;
    _objectBox.setAssetDimensionsFromBounding(GetModelBoundingBox(_asset.getModel()));
}

void StaticObject3D::setAsset(Asset2D)
{
    //convert 2d asset to 3d
}

void StaticObject3D::draw()
{
    DrawModel(_asset.getModel(), _objectBox.getPosition().convert(), _objectBox.getScale(), WHITE);
}

void StaticObject3D::move(Vector3D positionModifier)
{
    _objectBox.setPosition(_objectBox.getPosition() + positionModifier);
}

void StaticObject3D::moveTo(Vector3D newPosition)
{
    _objectBox.setPosition(newPosition);
}

void StaticObject3D::resizePercent(float percentage)
{
    _objectBox.setScale(percentage);
}

ObjectBox3D StaticObject3D::getBox()
{
    return _objectBox;
}