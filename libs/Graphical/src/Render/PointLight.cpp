/*
** EPITECH PROJECT, 2025
** Iso
** File description:
** PointLight
*/

#include "PointLight.hpp"

using namespace Render;

PointLight::PointLight(Vector3D position)
{
    const char *vsFilename = TextFormat("resources/shaders/glsl%i/lighting.vs", GLSL_VERSION);
    const char *fsFilename = TextFormat("resources/shaders/glsl%i/lighting.fs", GLSL_VERSION);
    Shader shader = LoadShader(vsFilename, fsFilename);

    _light = createLight(LIGHT_POINT, position, Vector3D(), WHITE, shader);
}

PointLight::PointLight(Vector3D position, Shader shader)
{
    _light = createLight(LIGHT_POINT, position, Vector3D(), WHITE, shader);
}

PointLight::PointLight(Vector3D position, Shader shader, Color color)
{
    _light = createLight(LIGHT_POINT, position, Vector3D(), color, shader);
}

PointLight::~PointLight()
{
}

Light PointLight::createLight(int type, Vector3D position, Vector3D target, Color color, Shader &shader)
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

void PointLight::updateLightValues(Shader &shader, Light light)
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
};

Light PointLight::getLight()
{
    return _light;
}
