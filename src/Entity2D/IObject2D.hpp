#pragma once

#include "../Utilities/Vector.hpp"
#include "../../Assets/Asset2D.hpp"
#include "../../Utilities/ObjectBox.hpp"

using namespace Utilities;

namespace Object2D {

    class IObject2D {
        public:
        virtual void setAsset(Asset2D) = 0;
        virtual void draw() = 0;
        virtual void move(Vector2D) = 0;
        virtual void moveTo(Vector2D) = 0;
        virtual void resizePercent(float) = 0;
        virtual ObjectBox2D getBox() = 0;
    };

}