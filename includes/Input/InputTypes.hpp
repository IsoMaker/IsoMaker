/*
** EPITECH PROJECT, 2024
** IsoMaker
** File description:
** InputTypes
*/

#pragma once

namespace input {
    enum Type {
        KEYBOARD,
        MOUSE,
        GAMEPAD,
    };

    enum State {
        NOTPRESSED,
        PRESSED,
        RELEASED,
        HELD,
    };

    enum Generic {
        UP,
        DOWN,
        LEFT,
        RIGHT,
        INTERACT,
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
