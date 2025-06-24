#include "BasicObject.hpp"
#include "rlgl.h"
#include <glad/glad.h>

using namespace Utilities;

BasicObject::BasicObject(Asset3D asset, Vector3D position) {
    setAsset(asset);
    moveTo(position);
    _assetType = AssetType::ASSET3D;
    _position = position;
}

BasicObject::BasicObject(Asset2D asset, Vector3D position, Vector3D dimension3D) {
    setAsset(asset, dimension3D);
    moveTo(position);
    _assetType = AssetType::ASSET2D;
}

void BasicObject::setAsset(Asset3D newAsset) {
    _asset3D = newAsset;
    _assetType = AssetType::ASSET3D;
    _objectBox.setAssetDimensionsFromBounding(GetModelBoundingBox(_asset3D.getModel()));
}

void BasicObject::setAsset(Asset2D newAsset, Vector3D dimension3D) {
    _assetType = AssetType::ASSET2D;
    _asset2D = newAsset;
    _objectBox.assetDimensions = dimension3D;
}

void BasicObject::setTexture(Asset2D asset, int frameWidth, int frameHeight, int totalFrames) {
    _asset2D = asset;
    _assetType = AssetType::ASSET2D;

    _frameWidth = frameWidth;
    _frameHeight = frameHeight;
    _totalFrames = totalFrames;

    _objectBox.assetDimensions = Vector3D((float)frameWidth, (float)frameHeight, 1.0f);
}

void BasicObject::updateAnimation() {
    _frameCounter++;
    if (_frameCounter >= _frameSpeed) {
        _frameCounter = 0;
        _currentFrame = (_currentFrame + 1) % _totalFrames;
    }
}

void BasicObject::draw() {
    if (_assetType == AssetType::ASSET3D) {
        DrawModel(_asset3D.getModel(), _objectBox.position.convert(), _objectBox.scale, WHITE);
    } else if (_assetType == AssetType::ASSET2D) {
        Rectangle source = {
            (float)(_currentFrame * _frameWidth),
            0,
            (float)_frameWidth,
            (float)_frameHeight
        };
        const float tileWidth = 64.0f;
        const float tileHeight = 32.0f;
        float isoX = (_objectBox.position.x - _objectBox.position.z) * tileWidth / 2.0f;
        float isoY = (_objectBox.position.x + _objectBox.position.z) * tileHeight / 2.0f - _objectBox.position.y * tileHeight;
        Vector2 position = {
            (isoX + SCREENWIDTH / 2) - _frameWidth,
            ((isoY + SCREENHEIGHT / 2) - _frameHeight) - 48
        };

        DrawTextureRec(_asset2D.getTexture(), source, position, WHITE);
    }
}

void BasicObject::drawWireframe() {
    if (_assetType != AssetType::ASSET3D) return;

    Mesh mesh = _asset3D.getModel().meshes[0]; // assume first mesh for now
    Vector3D pos = _objectBox.position;
    float scale = _objectBox.scale;

    rlPushMatrix();
    rlTranslatef(pos.x, pos.y, pos.z);
    rlScalef(scale, scale, scale);

    // Avoid z-fighting by pushing the wireframe forward
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1.0f, -1.0f);

    rlBegin(RL_LINES);
    rlColor4ub(0, 0, 0, 255); // yellow wireframe

    for (int i = 0; i < mesh.triangleCount * 3; i += 3) {
        int idx0 = mesh.indices[i];
        int idx1 = mesh.indices[i + 1];
        int idx2 = mesh.indices[i + 2];

        Vector3 v0 = { mesh.vertices[idx0 * 3 + 0], mesh.vertices[idx0 * 3 + 1], mesh.vertices[idx0 * 3 + 2] };
        Vector3 v1 = { mesh.vertices[idx1 * 3 + 0], mesh.vertices[idx1 * 3 + 1], mesh.vertices[idx1 * 3 + 2] };
        Vector3 v2 = { mesh.vertices[idx2 * 3 + 0], mesh.vertices[idx2 * 3 + 1], mesh.vertices[idx2 * 3 + 2] };

        rlVertex3f(v0.x, v0.y, v0.z);
        rlVertex3f(v1.x, v1.y, v1.z);

        rlVertex3f(v1.x, v1.y, v1.z);
        rlVertex3f(v2.x, v2.y, v2.z);

        rlVertex3f(v2.x, v2.y, v2.z);
        rlVertex3f(v0.x, v0.y, v0.z);
    }

    rlEnd();
    glDisable(GL_POLYGON_OFFSET_LINE); // Cleanup
    rlPopMatrix();
}

void BasicObject::resizeTo(float height) {
    float currentHeight = _objectBox.assetDimensions.z;
    float newScale = height / currentHeight;
    _objectBox.scale = newScale;
}

Vector3D BasicObject::getPosition()
{
    return _position;
}

Asset3D BasicObject::getAsset3D()
{
    return _asset3D;
}

Asset2D BasicObject::getAsset2D()
{
    return _asset2D;
}
