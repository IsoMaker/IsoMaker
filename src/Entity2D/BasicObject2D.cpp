#include "BasicObject2D.hpp"

using namespace Object2D;
using namespace Utilities;

BasicObject2D::BasicObject2D() {

}

BasicObject2D::BasicObject2D(Asset2D asset, Vector2D position) {
    setAsset(asset);
    moveTo(position);
}

BasicObject2D::~BasicObject2D() {

}

void BasicObject2D::setAsset(Asset2D newAsset) {
    _asset = newAsset;
    _objectBox.assetDimensions = {newAsset.getTexture().height, newAsset.getTexture().width};
}

void BasicObject2D::draw() {
    DrawTextureEx(_asset.getTexture(), _objectBox.position.convert(), 0, _objectBox.scale, WHITE);
}

void BasicObject2D::move(Vector2D positionModifier) {
    _objectBox.position = _objectBox.position + positionModifier;
}

void BasicObject2D::moveTo(Vector2D newPosition) {
    _objectBox.position = newPosition;
}

void BasicObject2D::resizePercent(float percentage) {
    _objectBox.scale = percentage;
}

ObjectBox2D BasicObject2D::getBox() {
    return _objectBox;
}