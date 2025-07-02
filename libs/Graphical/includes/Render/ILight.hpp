/*
** EPITECH PROJECT, 2025
** Iso
** File description:
** ILight
*/

#pragma once

#include "rlights.h"
#define GLSL_VERSION 330
#include "Vector.hpp"

using namespace Utilities;

namespace Render
{
    class ILight
    {
        public:
            virtual Vector3D getPosition() = 0;
            virtual void setPosition(Vector3D position) = 0;

    };
}
