/*
** EPITECH PROJECT, 2025
** Iso
** File description:
** Character
*/

#pragma once

#include "../../includes/Objects/AEntity.hpp"
#include "../Render/Camera.hpp"

namespace objects
{
    class Character : public AEntity
    {
        public:
            Character() : AEntity() {};
            Character(Asset2D asset);
            Character(Asset2D asset, Vector3D position);
            Character(Asset2D asset, Vector3D position, Vector2D framePosition, Vector2D frameSize);
            ~Character();

            bool isMoving();
            void setMoving(bool moving);

            void updateAnimation();

            void draw() { AEntity::draw(); };
            void draw(Vector2D tileSize);
        protected:
            int _totalFrames = 1;
            int _currentFrame = 0;
            int _frameCounter = 0;
            int _frameSpeed = 8;
            bool _moving = false;
    };
}
