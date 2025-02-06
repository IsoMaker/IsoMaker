/*
** EPITECH PROJECT, 2024
** IsoMaker
** File description:
** Mouse
*/

#include "Mouse.hpp"

using namespace input;

void MouseHandler::handleInput()
{
    _mouseCoords = GetMousePosition();

    for (auto it = _inputBindings.begin(); it != _inputBindings.end(); it++) {
        if (IsMouseButtonDown(it->first)) {
            updateState(it->second, State::PRESSED);
        } else {
            updateState(it->second, State::RELEASED);
        }
    }
}