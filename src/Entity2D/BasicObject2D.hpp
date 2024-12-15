#pragma once

#include "IObject2D.hpp"
#include "../Assets/Asset2D.hpp"
#include "../Utilities/ObjectBox.hpp"

using namespace Utilities;

namespace Object2D {

    class BasicObject2D : public IObject2D {
        public:
            BasicObject2D();
            BasicObject2D(Asset2D, Vector2D);
            ~BasicObject2D();

            void setAsset(Asset2D);
            void draw();
            void move(Vector2D);
            void moveTo(Vector2D);
            void resizePercent(float);
            ObjectBox2D getBox();

        private:
            ObjectBox2D _objectBox;
            Asset2D _asset;
    };

}