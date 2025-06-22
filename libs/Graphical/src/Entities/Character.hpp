/*
** EPITECH PROJECT, 2025
** Iso
** File description:
** Character
*/

#pragma once

#include "AEntity.hpp"

namespace objects
{
    class Character : public AEntity
    {
        public:
            using AEntity::AEntity;
            ~Character() = default;

            void draw() override;
    };
}