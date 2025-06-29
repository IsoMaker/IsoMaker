/*
** EPITECH PROJECT, 2025
** Iso
** File description:
** AObject
*/

#pragma once

#include "IObject.hpp"

using namespace Utilities;

namespace objects
{
    class AObject : public IObject
    {
        public:
            AObject()
            {
                _asset2D = Asset2D();
                _box2D = ObjectBox2D();
            }
            AObject(Asset2D asset2D)
            {
                _asset2D = asset2D;
                Vector2D position = Vector2D(0, 0);
                Texture2D texture = asset2D.getTexture();
                Vector2D size = Vector2D((float)texture.width, (float)texture.height);
                _box2D = ObjectBox2D(position, size);
            };
            AObject(Asset2D asset2D, Vector2D position)
            {
                _asset2D = asset2D;
                Texture2D texture = asset2D.getTexture();
                Vector2D size = Vector2D((float)texture.width, (float)texture.height);
                _box2D = ObjectBox2D(position, size);
            };
            AObject(Asset2D asset2D, Vector2D position, Vector2D size)
            {
                _asset2D = asset2D;
                _box2D = ObjectBox2D(position, size);
            };
            AObject(Asset2D asset2D, Vector2D position, Vector2D size, float scale)
            {
                _asset2D = asset2D;
                _box2D = ObjectBox2D(position, size, scale);
            };

            ~AObject() = default;

            Asset2D getAsset2D() const { return _asset2D; };
            void setAsset2D(Asset2D asset2D) { _asset2D = asset2D; };

            ObjectBox2D &getBox2D() { return _box2D; };
            void setBox2DPosition(Vector2D position) { _box2D.setPosition(position); };
            void setBox2DSize(Vector2D size) { _box2D.setSize(size); };
            void setBox2DScale(float scale) { _box2D.setScale(scale); };

            void draw()
            {
                Rectangle source = _box2D.getRectangle();
                DrawTextureRec(_asset2D.getTexture(), source, _box2D.getPosition().convert(), WHITE);
            }

        protected:
            Asset2D _asset2D;
            ObjectBox2D _box2D;

        private:
    };
}
