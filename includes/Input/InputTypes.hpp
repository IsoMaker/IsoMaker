/*
** EPITECH PROJECT, 2024
** IsoMaker
** File description:
** InputTypes
*/

#pragma once

namespace input
{
    enum class Type
    {
        KEYBOARDMOUSE,
        GAMEPAD,
    };

    enum class State
    {
        NOTPRESSED,
        PRESSED,
        RELEASED,
        HELD,
    };

    enum class Generic
    {
        UP,
        DOWN,
        LEFT,
        RIGHT,
        INTERACT1,
        INTERACT2,
        INTERACT3,
        INTERACT4,
        ATTACK,
        INVENTORY,
        PAUSE,
        ENTER,
        SELECT1,
        SELECT2,
        SELECT3,
        SELECT4,
        VOID,
    };
}
