/*
** EPITECH PROJECT, 2025
** Iso
** File description:
** ILight
*/

#pragma once

#include "raylib.h"
#include "Utilities/Vector.hpp"

#define GLSL_VERSION 330

using namespace Utilities;

namespace Render
{
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

    // Send light properties to shader
    // NOTE: Light shader locations should be available 
    void updateLightValues(Shader shader, Light light)
    {
        // Send to shader light enabled state and type
        SetShaderValue(shader, light.enabledLoc, &light.enabled, SHADER_UNIFORM_INT);
        SetShaderValue(shader, light.typeLoc, &light.type, SHADER_UNIFORM_INT);
    
        // Send to shader light position values
        float position[3] = { light.position.x, light.position.y, light.position.z };
        SetShaderValue(shader, light.positionLoc, position, SHADER_UNIFORM_VEC3);
    
        // Send to shader light target position values
        float target[3] = { light.target.x, light.target.y, light.target.z };
        SetShaderValue(shader, light.targetLoc, target, SHADER_UNIFORM_VEC3);
    
        // Send to shader light color values
        float color[4] = { (float)light.color.r/(float)255, (float)light.color.g/(float)255, 
                           (float)light.color.b/(float)255, (float)light.color.a/(float)255 };
        SetShaderValue(shader, light.colorLoc, color, SHADER_UNIFORM_VEC4);
    }

    Light createLight(int type, Vector3D position, Vector3D target, Color color, Shader shader)
    {
        Light light = { 0 };
    
        light.enabled = true;
        light.type = type;
        light.position = position.convert();
        light.target = target.convert();
        light.color = color;
        updateLightValues(shader, light);

        return light;
    }

    class ILight
    {
        public:
            virtual Light getLight() = 0;

    };
}
