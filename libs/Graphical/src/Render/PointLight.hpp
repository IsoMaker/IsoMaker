/*
** EPITECH PROJECT, 2025
** Iso
** File description:
** PointLight
*/

#pragma once

#include "Utilities/Vector.hpp"

using namespace Utilities;

namespace Render
{
    #define GLSL_VERSION 330

    typedef struct {   
        int type;
        bool enabled;
        Vector3 position;
        Vector3 target;
        Color color;
        float attenuation;
        
        // Shader locations
        int enabledLoc;
        int typeLoc;
        int positionLoc;
        int targetLoc;
        int colorLoc;
        int attenuationLoc;
    } Light;
    
    // Light type
    typedef enum {
        LIGHT_DIRECTIONAL = 0,
        LIGHT_POINT
    } LightType;

    class PointLight
    {
        public:
            PointLight(Vector3D position);
            PointLight(Vector3D position, Shader shader);
            PointLight(Vector3D position, Shader shader, Color color);
            ~PointLight();

            Light getLight();

        private:
            Light _light;

            Light createLight(int type, Vector3D position, Vector3D target, Color color, Shader &shader);
            void updateLightValues(Shader &shader, Light light);
    };
}
