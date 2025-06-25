/*
** EPITECH PROJECT, 2025
** Iso
** File description:
** Character
*/

#pragma once

#include "../../includes/Objects/AEntity.hpp"

namespace objects
{
    class Character : public AEntity
    {
        public:
            using AEntity::AEntity;
            ~Character();

            void draw() { AEntity::draw(); };
            void draw(Camera &camera);
    };
}
