#include "Camera.hpp"

using namespace Utilities;

Render::Camera::Camera() {
    _angle = 0;
    _cameraAngles = {
        {0, Vector2D(1, 1)},
        {90, Vector2D(1, -1)},
        {180, Vector2D(-1, -1)},
        {270, Vector2D(-1, 1)},
    };

    _distance = 18;
    _height = 12;

    _target = Vector3D(0, 0, 0);
    _camera.up = Vector3D(0, 1, 0).convert();
    _camera.fovy = 22.5f;
    _camera.projection = CAMERA_ORTHOGRAPHIC;
    updateCamera();
}

void Render::Camera::rotateClock() {
    _angle += 90;
    if (_angle > 270)
        _angle = 0;
    updateCamera();
}

void Render::Camera::rotateCounterclock() {
    _angle -= 90;
    if (_angle < 0)
        _angle = 270;
    updateCamera();
}

void Render::Camera::rotate(float angle) {
    if (_cameraAngles.find(angle) != _cameraAngles.end())
        _angle = angle;
}

void Render::Camera::updateCamera() {
    Vector2D angleModifier = _cameraAngles[_angle];

    _position.x = _target.x + (_distance * angleModifier.x);
    _position.z = _target.z + (_distance * angleModifier.y);
    _position.y = _target.y + _height;
    _camera.target = _target.convert();
    _camera.position = _position.convert();
}
