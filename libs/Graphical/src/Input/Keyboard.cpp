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
    for (auto bind = _inputBindings.begin(); bind != _inputBindings.end(); bind++) {
        if (IsKeyDown(bind->first)) {
            updateState(bind->second, State::PRESSED);
        } else if (_inputStates.at(bind->second) == State::PRESSED) {
            updateState(bind->second, State::RELEASED);
        } else {
            updateState(bind->second, State::NOTPRESSED);
        }
    }
}
