#pragma once

#include "../Utilities/Vector.hpp"

using namespace Utilities;

namespace Object3D {

    class IObject3D {
        public:
            virtual void setAsset(Asset3D) = 0;
            virtual void draw() = 0;
            virtual void move(Vector3D) = 0;
            virtual void moveTo(Vector3D) = 0;
            virtual void resizePercent(float) = 0;
            virtual ObjectBox3D getBox() = 0;
    };

}