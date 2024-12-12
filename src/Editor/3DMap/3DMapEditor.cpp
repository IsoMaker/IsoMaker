#include "3DMapEditor.hpp"
#include "raylib.h"

MapEditor::MapEditor(Render::Camera &camera, Render::Window &window) : _window(window), _camera(camera) {
    _cubeHeight = 1;
}

MapEditor::~MapEditor() {

}

void MapEditor::update() {
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        Vector2D mousePos = GetMousePosition();
        Vector3D click = alignPosition(mousePos);
        if (_objects3D.size() == 0 || click != Vector3D(0, 0, 0))
            addCube(click);
    }
    //get input and change things according to it
}

void MapEditor::drawAll() {
    _window.startRender();
    _window.clearBackground(SKYBLUE);
    _camera.start3D();
    draw3DElements();
    _camera.end3D();
    draw2DElements();
    _window.endRender();
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
    Object3D::BasicObject3D newObject = Object3D::BasicObject3D(_currentCubeType, position);
    newObject.resizeTo(_cubeHeight);
    _objects3D.push_back(newObject);
}

void MapEditor::removeCube(Vector3D position) {

}

Vector3D MapEditor::alignPosition( Vector2D mousePos) {
    Ray ray = GetMouseRay(mousePos.convert(), _camera.getRaylibCam());

    RayCollision closestHit = { false, std::numeric_limits<float>::max(), { 0, 0, 0 }, { 0, 0, 0 } };
    Object3D::BasicObject3D closestObject;

    Vector3D result = Vector3D(0, 0, 0);

    for (auto i = _objects3D.begin(); i != _objects3D.end(); i++) {
        RayCollision collision = GetRayCollisionBox(ray, i->getBox().convert());
        if (collision.hit && collision.distance < closestHit.distance) {
            closestHit = collision;
            closestObject = *i;
        }
    }

    std::cout << "Final block clicked (" << closestObject.getBox().position.x << ", " << closestObject.getBox().position.y << ", " << closestObject.getBox().position.z << ")" << std::endl;
    if (closestHit.hit) {
        ObjectBox3D modelBox = closestObject.getBox();
        Vector3D collisionPoint = closestHit.point;
        Vector3D diff = collisionPoint - modelBox.position;
        if (diff.x >= _cubeHeight)  {result = Vector3D(modelBox.position.x + _cubeHeight, modelBox.position.y, modelBox.position.z); std::cout << "1" << std::endl;}
        if (diff.x <= -_cubeHeight) {result = Vector3D(modelBox.position.x - _cubeHeight, modelBox.position.y, modelBox.position.z); std::cout << "2" << std::endl;}
        if (diff.z >= _cubeHeight)  {result = Vector3D(modelBox.position.x, modelBox.position.y, modelBox.position.z + _cubeHeight); std::cout << "3" << std::endl;}
        if (diff.z <= -_cubeHeight) {result = Vector3D(modelBox.position.x, modelBox.position.y, modelBox.position.z - _cubeHeight); std::cout << "4" << std::endl;}
        if (diff.y >= _cubeHeight)  {result = Vector3D(modelBox.position.x, modelBox.position.y + _cubeHeight, modelBox.position.z); std::cout << "5" << std::endl;}
        std::cout << "Click detected at (" << collisionPoint.x << ", " << collisionPoint.y << ", " << collisionPoint.z << ")" << std::endl;
        std::cout << "Diff detected at (" << diff.x << ", " << diff.y << ", " << diff.z << ")" << std::endl;
        std::cout << "Adding block at coords (" << result.x << ", " << result.y << ", " << result.z << ")" << std::endl;
        return result;
    }
    return result;
}