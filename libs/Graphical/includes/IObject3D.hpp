#pragma once

#include "../src/Utilities/Vector.hpp"
#include "../src/Assets/Asset2D.hpp"
#include "../src/Assets/Asset3D.hpp"
#include "../src/Utilities/ObjectBox.hpp"

using namespace Utilities;

class IObject3D {
    public:
        virtual void setAsset(Asset3D) = 0;
        virtual void setAsset(Asset2D, Vector3D dimension3D) = 0;
        virtual void draw() = 0;
        virtual void move(Vector3D) = 0;
        virtual void moveTo(Vector3D) = 0;
        virtual void resizePercent(float) = 0;
        virtual ObjectBox3D getBox() = 0;
};
