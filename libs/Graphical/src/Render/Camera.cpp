#include "Camera.hpp"

using namespace Utilities;

Render::Camera::Camera() {
    _cameraAngles = {
    {0, Vector2D(1, 1)},
    {90, Vector2D(1, -1)},
    {180, Vector2D(-1, -1)},
    {270, Vector2D(-1, 1)},
    {-90, Vector2D(-1, 1)},
    };

    _target = Vector3D(0, 0, 0);
    _distance = 5;
    _height = 5;
    _angle = 0;

    _camera.up = Vector3D(0, 1, 0).convert();
    _camera.fovy = 55;
    _camera.projection = CAMERA_ORTHOGRAPHIC;
    updateCamera();
}

void Render::Camera::rotate() {
    _angle += 90;
    if (_angle > 270)
        _angle = 0;
}

void Render::Camera::rotate(float angle) {
    if (_cameraAngles.find(angle) != _cameraAngles.end())
        _angle = angle;
}

void Render::Camera::updateCamera() {
    Vector2D angleModifier = _cameraAngles[_angle];
    _position = Vector3D(_target.x + _distance, _target.y + _height, _target.z + _distance);
    _position = _position * angleModifier;
    _camera.target = _target.convert();
    _camera.position = _position.convert();
}
