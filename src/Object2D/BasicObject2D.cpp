#include "BasicObject2D.hpp"

using namespace Object2D;
using namespace Utilities;

BasicObject2D::BasicObject2D(Asset2D asset, Vector2D position) {
    setAsset(asset);
    moveTo(position);
}

void BasicObject2D::setAsset(Asset2D newAsset) {
    _asset = newAsset;
    _objectBox.assetDimensions = {(float)newAsset.getTexture().width, (float)newAsset.getTexture().height};
}

void BasicObject2D::draw() {
    DrawTextureEx(_asset.getTexture(), _objectBox.position.convert(), 0, _objectBox.scale, WHITE);
}

bool BasicObject2D::isInObject(Vector2D pos) {
    bool result = _objectBox.isInBox(pos);
    return result;
}