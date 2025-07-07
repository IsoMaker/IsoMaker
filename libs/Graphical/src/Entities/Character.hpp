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

            int getTotalFrames() { return _totalFrames; };
            void setTotalFrames(int frames) { _totalFrames = frames; };

            void updateAnimation();

            void draw() { AEntity::draw(); };
            void draw(Rectangle renderArea, std::shared_ptr<Render::Camera> camera);
            void draw(Vector3D tmp);
        protected:
            int _totalFrames = 1;
            int _currentFrame = 0;
            int _frameCounter = 0;
            int _frameSpeed = 8;
            bool _isMoving = false;
    };
}
