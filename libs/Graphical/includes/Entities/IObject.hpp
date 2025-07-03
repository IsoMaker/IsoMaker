/*
** EPITECH PROJECT, 2025
** Iso
** File description:
** IObject
*/

#pragma once

#include "Assets/Asset2D.hpp"
#include "Utilities/Vector.hpp"
#include "Utilities/ObjectBox.hpp"

#define SCREENHEIGHT 1200
#define SCREENWIDTH 1600

using namespace Utilities;

namespace objects
{
    class IObject
    {
        public:
            virtual Asset2D getAsset2D() const = 0;
            virtual void setAsset2D(Asset2D) = 0;

            virtual ObjectBox2D &getBox2D() = 0;

            virtual void draw() = 0;
    };
}
