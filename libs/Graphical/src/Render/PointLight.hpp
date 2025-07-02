/*
** EPITECH PROJECT, 2025
** Iso
** File description:
** PointLight
*/

#pragma once

#include "Render/ILight.hpp"

using namespace Utilities;

namespace Render
{
    class PointLight : public ILight
    {
        public:
            PointLight(Vector3D position);
            PointLight(Vector3D position, Shader shader);
            PointLight(Vector3D position, Shader shader, Color color);
            ~PointLight();

            Light getLight();

        protected:
            Light _light;
    };
}
