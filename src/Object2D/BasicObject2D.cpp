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
    std::cout << "Pos " << pos.x << " " << pos.y << std::endl;
    std::cout << "Position " << _objectBox.position.x << " " << _objectBox.position.y << std::endl;
    std::cout << "Dimensions " << _objectBox.assetDimensions.x << " " << _objectBox.assetDimensions.y << std::endl;
    std::cout << std::boolalpha << (pos > _objectBox.position) << std::endl;
    std::cout << std::boolalpha << (pos < (_objectBox.position + (_objectBox.assetDimensions * _objectBox.scale))) << std::endl;
    bool result = _objectBox.isInBox(pos);
    std::cout << std::boolalpha << result << std::endl;
    return result;
}