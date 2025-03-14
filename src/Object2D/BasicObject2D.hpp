#pragma once

#include "../../includes/IObject2D.hpp"
#include "../Assets/Asset2D.hpp"
#include "../Utilities/ObjectBox.hpp"

using namespace Utilities;

class BasicObject2D : public IObject2D {
    public:
        BasicObject2D() {};
        BasicObject2D(Asset2D, Vector2D);
        ~BasicObject2D() {};
        void setAsset(Asset2D);
        void draw();
        void move(Vector2D positionModifier) {_objectBox.position = _objectBox.position + positionModifier;};
        void moveTo(Vector2D newPosition) {_objectBox.position = newPosition;};
        void resizePercent(float percentage) {_objectBox.scale = percentage;};
        bool isInObject(Vector2D pos);
        ObjectBox2D getBox() {return _objectBox;};
    private:
        ObjectBox2D _objectBox;
        Asset2D _asset;
};
