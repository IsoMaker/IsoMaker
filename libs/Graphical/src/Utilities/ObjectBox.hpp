#pragma once

#include "Vector.hpp"
#include "raylib.h"

namespace Utilities
{
    template <class T>
    class ObjectBox
    {
        public:
            ObjectBox() {};
            ObjectBox(T position, T size, float scale = 1.0f) : _position(position), _size(size), _scale(scale) {};
            ~ObjectBox() = default;

            T getPosition() const { return _position; };
            void setPosition(T newPosition) {
                _position = newPosition;
            }
            virtual void setPosX(float x) = 0;
            virtual void setPosY(float y) = 0;
            void movePosition(T positionModifier) { _position += positionModifier; };

            T getSize() const { return _size; };
            void setSize(T newSize) { _size = newSize; };
            void resizeByPercent(float percent) { _size *= percent; };

            float getScale() const { return _scale; };
            void setScale(float newScale) { _scale = newScale; };

        protected:
            T _position;  // position of object
            T _size;      // size of hitbox of object
            float _scale; // render scale of object

        private:
    };

    class ObjectBox2D : public ObjectBox<Vector2D> {
        public:
            using ObjectBox::ObjectBox;

            void setPosX(float x) { _position.x = x; };
            void setPosY(float y) { _position.y = y; };

            Rectangle getRectangle()
            {
                return { _position.x, _position.y, _size.x, _size.y };
            }

            bool isInBox(Vector2D pos)
            {
                if (pos > _position && pos < (_position + (_size * _scale)))
                    return true;
                return false;
            }

        protected:
        private:
    };

    class ObjectBox3D : public ObjectBox<Vector3D> {
        public:
            using ObjectBox::ObjectBox;

            void setPosX(float x) { _position.x = x; };
            void setPosY(float y) { _position.y = y; };
            void setPosZ(float z) { _position.z = z; };

            void setSizeFromBounding(BoundingBox box) { _size = Vector3D(box.max) - Vector3D(box.min); };

            BoundingBox convert()
            {
                Vector3 min = _position.convert();
                Vector3 max = ((_position + _size) * _scale).convert();
                BoundingBox box = { min, max };
                return box;
            };

        protected:
        private:
    };
}
