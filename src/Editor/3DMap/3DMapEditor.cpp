#include "3DMapEditor.hpp"
#include "raylib.h"

MapEditor::MapEditor(Render::Camera &camera, Render::Window &window) : _window(window), _camera(camera) {
    _cubeHeight = 1;
}

MapEditor::~MapEditor() {

}

void MapEditor::update(input::MouseHandler &mouseHandler) {
    if (mouseHandler.isReleased(input::Generic::SELECT1)) {
        Vector2D mousePos = mouseHandler.getMouseCoords();
        Vector3D click = alignPosition(mousePos);
        if (_objects3D.size() == 0 || click != Vector3D(0, 0, 0))
            addCube(click);
    }
}

void MapEditor::draw2DElements() {
    for (auto i = _objects2D.begin(); i != _objects2D.end(); i++)
        i->draw();
}

void MapEditor::draw3DElements() {
    for (auto i = _objects3D.begin(); i != _objects3D.end(); i++)
        i->draw();
    DrawGrid(10, 1.0f);
}

void MapEditor::changeCubeType(Asset3D newAsset) {
    _currentCubeType = newAsset;
}

void MapEditor::addCube(Vector3D position) {
    BasicObject3D newObject = BasicObject3D(_currentCubeType, position);
    newObject.resizeTo(_cubeHeight);
    _objects3D.push_back(newObject);
}

void MapEditor::removeCube(std::vector<BasicObject3D>::iterator toRemove) {
    _objects3D.erase(toRemove);
}

std::pair<Vector3D, std::vector<BasicObject3D>::iterator> MapEditor::alignPosition( Vector2D mousePos) {
    Ray ray = GetMouseRay(mousePos.convert(), _camera.getRaylibCam());

    RayCollision closestHit = { false, std::numeric_limits<float>::max(), { 0, 0, 0 }, { 0, 0, 0 } };

    std::pair<Vector3D, std::vector<BasicObject3D>::iterator> result = std::make_pair<Vector3D, std::vector<BasicObject3D>::iterator>(Vector3D(0, 0, 0), _objects3D.begin());

    for (auto i = _objects3D.begin(); i != _objects3D.end(); i++) {
        RayCollision collision = GetRayCollisionBox(ray, i->getBox().convert());
        if (collision.hit && collision.distance < closestHit.distance) {
            closestHit = collision;
            result.second = i;
        }
    }

    if (closestHit.hit) {
        ObjectBox3D modelBox = result.second->getBox();
        Vector3D collisionPoint = closestHit.point;
        Vector3D diff = collisionPoint - modelBox.position;
        if (diff.x >= _cubeHeight)  {result.first = Vector3D(modelBox.position.x + _cubeHeight, modelBox.position.y, modelBox.position.z);}
        if (diff.x <= -_cubeHeight) {result.first = Vector3D(modelBox.position.x - _cubeHeight, modelBox.position.y, modelBox.position.z);}
        if (diff.z >= _cubeHeight)  {result.first = Vector3D(modelBox.position.x, modelBox.position.y, modelBox.position.z + _cubeHeight);}
        if (diff.z <= -_cubeHeight) {result.first = Vector3D(modelBox.position.x, modelBox.position.y, modelBox.position.z - _cubeHeight);}
        if (diff.y >= _cubeHeight)  {result.first = Vector3D(modelBox.position.x, modelBox.position.y + _cubeHeight, modelBox.position.z);}
        return result;
    }
    return result;
}