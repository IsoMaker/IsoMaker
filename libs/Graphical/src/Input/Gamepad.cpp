/*
** EPITECH PROJECT, 2024
** IsoMaker
** File description:
** Controller
*/

#include "Gamepad.hpp"

using namespace input;

void GamepadHandler::handleInput()
{
    Vector2 mousePos = GetMousePosition();
    _cursorCoords = Utilities::Vector2D(mousePos.x, mousePos.y);

    for (auto bind = _inputBindings.begin(); bind != _inputBindings.end(); bind++) {
        if (IsGamepadButtonDown(0, bind->first)) {
            updateState(bind->second, State::PRESSED);
        } else if (isPressed(bind->second)) {
            updateState(bind->second, State::RELEASED);
        } else {
            updateState(bind->second, State::NOTPRESSED);
        }
    }
}
