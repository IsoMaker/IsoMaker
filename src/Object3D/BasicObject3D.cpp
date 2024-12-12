#include "BasicObject3D.hpp"

using namespace Object3D;
using namespace Utilities;

BasicObject3D::BasicObject3D(Asset3D asset, Vector3D position) {
    setAsset(asset);
    moveTo(position);
    _assetType = AssetType::ASSET3D;
}

BasicObject3D::BasicObject3D(Asset2D asset, Vector3D position, Vector3D dimension3D) {
    setAsset(asset, dimension3D);
    moveTo(position);
    _assetType = AssetType::ASSET2D;
}

void BasicObject3D::setAsset(Asset3D newAsset) {
    _asset3D = newAsset;
    _assetType = AssetType::ASSET3D;
    _objectBox.setAssetDimensionsFromBounding(GetModelBoundingBox(_asset3D.getModel()));
}

void BasicObject3D::setAsset(Asset2D newAsset, Vector3D dimension3D) {
    _assetType = AssetType::ASSET2D;
    _asset2D = newAsset;
    _objectBox.assetDimensions = dimension3D;
}

void BasicObject3D::draw() {
    if (_assetType == AssetType::ASSET3D) {
        DrawModel(_asset3D.getModel(), _objectBox.position.convert(), _objectBox.scale, WHITE);
    } else if (_assetType == AssetType::ASSET2D) {
        DrawCubeTexture(_asset2D.getTexture(), _objectBox.position.convert(), _objectBox.assetDimensions.x, _objectBox.assetDimensions.y, _objectBox.assetDimensions.z, WHITE);
    }
}

void BasicObject3D::resizeTo(float height) {
    float currentHeight = _objectBox.assetDimensions.z;
    float newScale = height / currentHeight;
    _objectBox.scale = newScale;
}