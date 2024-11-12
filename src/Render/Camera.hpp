#pragma once

#include "../Utilities/Vector.hpp"
#include <map>

using namespace Utilities;

namespace Render {

    class Camera {
        public:
            Camera();
            ~Camera();

            void start3D();
            void end3D();

            void setTarget(Vector3D);
            void setDistance(float);
            void setHeight(float);
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