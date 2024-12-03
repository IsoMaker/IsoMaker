#include "ObjectBox.hpp"

using namespace Utilities;

ObjectBox2D::ObjectBox2D() {
    position = Vector2D(0, 0);
    assetDimensions = Vector2D(0, 0);
    scale = 1;
}

ObjectBox2D::ObjectBox2D(Vector2D position, Vector2D dimensions, float scale) {
    position = position;
    assetDimensions = dimensions;
    scale = scale;
}

ObjectBox2D::ObjectBox2D(const ObjectBox2D& other) {
    position = other.position;
    assetDimensions = other.assetDimensions;
    scale = other.scale;
}

ObjectBox2D::~ObjectBox2D() {
}


ObjectBox3D::ObjectBox3D() {
    position = Vector3D(0, 0, 0);
    assetDimensions = Vector3D(0, 0, 0);
    scale = 1;
}

ObjectBox3D::ObjectBox3D(Vector3D position, Vector3D dimensions, float scale) {
    position = position;
    assetDimensions = dimensions;
    scale = scale;
}

ObjectBox3D::ObjectBox3D(const ObjectBox3D& other) {
    position = other.position;
    assetDimensions = other.assetDimensions;
    scale = other.scale;
}

ObjectBox3D::~ObjectBox3D() {
}


void ObjectBox3D::setAssetDimensionsFromBounding(BoundingBox box) { //maybe there will be a bug where the dimensions will be negative
    assetDimensions = Vector3D(box.max) - Vector3D(box.min);
}

BoundingBox ObjectBox3D::convert() {
    BoundingBox temp;
    temp.min = position.convert();
    temp.max = (position + (assetDimensions * scale)).convert();
    return temp;
}
