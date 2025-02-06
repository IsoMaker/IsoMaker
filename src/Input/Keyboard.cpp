/*
** EPITECH PROJECT, 2024
** IsoMaker
** File description:
** Keyboard
*/

#include "Keyboard.hpp"

using namespace input;

void KeyboardHandler::handleInput()
{
    for (auto it = _inputBindings.begin(); it != _inputBindings.end(); it++) {
        if (IsKeyDown(it->first)) {
            updateState(it->second, State::PRESSED);
        } else {
            updateState(it->second, State::RELEASED);
        }
    }
}
