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

    _light = CreateLight(LIGHT_POINT, position.convert(), Vector3Zero(), WHITE, shader);
}

PointLight::PointLight(Vector3D position, Shader shader)
{
    _light = CreateLight(LIGHT_POINT, position.convert(), Vector3Zero(), WHITE, shader);
}

PointLight::PointLight(Vector3D position, Shader shader, Color color)
{
    _light = CreateLight(LIGHT_POINT, position.convert(), Vector3Zero(), color, shader);
}

PointLight::~PointLight()
{
}

Light PointLight::getLight()
{
    return _light;
}
