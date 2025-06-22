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
            AObject(Asset2D asset2D, Vector2D position, Vector2D size, float scale = 1.0f)
            {
                _asset2D = asset2D;
                _box2D = ObjectBox2D(position, size, scale);
            };

            ~AObject() = default;

            Asset2D getAsset2D() const { return _asset2D; };
            void setAsset2D(Asset2D asset2D) { _asset2D = asset2D; };

            ObjectBox2D &getBox2D() { return _box2D; };

            void draw()
            {
                Vector2D position = _box2D.getPosition();
                DrawTexture(_asset2D.getTexture(), position.x, position.y, WHITE);
            };

        protected:
            Asset2D _asset2D;
            ObjectBox2D _box2D;

        private:
    };
}
