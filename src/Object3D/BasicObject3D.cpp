#include "BasicObject3D.hpp"

using namespace Utilities;

BasicObject3D::BasicObject3D(Asset3D asset, Vector3D position) {
    setAsset(asset);
    _assetType = AssetType::ASSET3D;
    moveTo(position);
}

BasicObject3D::BasicObject3D(Asset2D asset, Vector3D position, Vector3D dimension3D) {
    setAsset(asset, dimension3D);
    _assetType = AssetType::ASSET2D;
    moveTo(position);
}

void BasicObject3D::setAsset(Asset3D newAsset) {
    _asset3D = newAsset;
    _assetType = AssetType::ASSET3D;
    _objectBox.setAssetDimensionsFromBounding(GetModelBoundingBox(_asset3D.getModel()));
}

void BasicObject3D::setAsset(Asset2D newAsset, Vector3D dimension3D) {
    _asset2D = newAsset;
    _assetType = AssetType::ASSET2D;
    _objectBox.assetDimensions = dimension3D;
}

void BasicObject3D::draw() {
    if (_assetType == AssetType::ASSET3D) {
        DrawModel(_asset3D.getModel(), _objectBox.position.convert(), _objectBox.scale, WHITE);
    } else if (_assetType == AssetType::ASSET2D) {
        DrawCubeTexture(_asset2D.getTexture(), _objectBox.position.convert(), _objectBox.assetDimensions.x, _objectBox.assetDimensions.y, _objectBox.assetDimensions.z, WHITE);
    }
}

void BasicObject3D::drawWireframe() {
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

void BasicObject3D::resizeTo(float height) {
    float currentHeight = _objectBox.assetDimensions.z;
    float newScale = height / currentHeight;
    _objectBox.scale = newScale;
}
