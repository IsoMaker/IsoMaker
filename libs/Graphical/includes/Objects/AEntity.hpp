/*
** EPITECH PROJECT, 2025
** Iso
** File description:
** AEntity
*/

#pragma once

#include "AObject.hpp"

namespace objects
{
    class AEntity : public AObject
    {
        public:
            AEntity() : AObject()
            {
                _box3D = ObjectBox3D();
            }
            AEntity(Asset2D asset2D) : AObject(asset2D)
            {
                _box3D = ObjectBox3D();
            };
            AEntity(Asset2D asset2D, Vector3D position)  : AObject(asset2D)
            {
                Texture2D texture = asset2D.getTexture();
                Vector3D size = Vector3D((float)texture.width, (float)texture.height, (float)texture.width);
                _box3D = ObjectBox3D(position, size);
            };
            AEntity(Asset2D asset2D, Vector3D position, Vector3D size) : AObject(asset2D)
            {
                _box3D = ObjectBox3D(position, size);
            };
            AEntity(Asset2D asset2D, Vector3D position, Vector3D size, float scale) : AObject(asset2D)
            {
                _box3D = ObjectBox3D(position, size, scale);
            };

            ~AEntity() = default;

            Vector3D getBoxPosition() { return _box3D.getPosition(); };
            ObjectBox3D &getBox3D() { return _box3D; };
            void setBox3DPosition(Vector3D position) { _box3D.setPosition(position); };
            void setBox3DSize(Vector3D size) { _box3D.setSize(size); };
            void setBox3DScale(float scale) { _box3D.setScale(scale); };

            // virtual void update() = 0;

        protected:
            ObjectBox3D _box3D;

        private:
    };
}
