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
            ~ObjectBox() {};

            T getPosition() const { return _position; };
            void setPosition(T newPosition ) { _position = newPosition; };
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

            void setPosition(Vector2D newPosition) { _position = Vector3D(newPosition.x, 0, newPosition.y); };
            void movePosition(Vector2D positionModifier)
            {
                Vector3D newPos = _position + positionModifier;
                _position = newPos;
            };

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

