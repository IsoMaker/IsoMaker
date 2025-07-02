#pragma once

#include <map>
#include <unordered_map>

#include "Utilities/Vector.hpp"

using namespace Utilities;

namespace Render {

    class Camera {
        public:
            Camera();
            ~Camera() { std::cout << "Camera destroyed" << std::endl; };

            void start3D() { BeginMode3D(_camera); };
            void end3D() { EndMode3D(); };

            Vector3D getPosition() { return _position; };
            Vector3D getTarget() { return _target; };
            int getAngle() { return _angle; };

            void setTarget(Vector3D target) {_target = target;};
            void setDistance(float distance) {_distance = distance;};
            void setHeight(float height) {_height = height;};

            Camera3D &getRaylibCam() { return _camera; };

            void rotateClock();
            void rotateCounterclock();
            void rotate(float angle);
            void updateCamera();
        protected:
        private:
            Vector3D _position;
            Vector3D _target;
            int _distance;
            int _height;
            int _angle;
            Camera3D _camera;
            std::unordered_map<int, Vector2D> _cameraAngles;
    };

}
