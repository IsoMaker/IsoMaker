/*
** EPITECH PROJECT, 2025
** Iso
** File description:
** Character
*/

#include "Character.hpp"

using namespace objects;

Character::~Character()
{
}

void Character::draw(Camera &camera)
{
    DrawBillboard(camera, getAsset2D().getTexture(), getBox3D().getPosition().convert(), getBox3D().getScale(), WHITE);
}
