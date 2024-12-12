#pragma once

#include "../Utilities/Vector.hpp"
#include <map>

using namespace Utilities;

namespace Render {

    class Camera {
        public:
            Camera();
            ~Camera() {};

            void start3D() {BeginMode3D(_camera);};
            void end3D() {EndMode3D();};

            void setTarget(Vector3D target) {_target = target;};
            void setDistance(float distance) {_distance = distance;};
            void setHeight(float height) {_height = height;};

            Camera3D getRaylibCam() {return _camera;};

            void rotate();
            void rotate(float angle);
            void updateCamera();

        protected:
            Vector3D _position;
            Vector3D _target;
            int _distance;
            int _height;
            int _angle;
            Camera3D _camera;
            std::unordered_map<int, Vector2D> _cameraAngles;
    };

}
