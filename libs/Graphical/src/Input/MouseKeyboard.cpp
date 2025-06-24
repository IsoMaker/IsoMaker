/*
** EPITECH PROJECT, 2024
** IsoMaker
** File description:
** Mouse
*/

#include "MouseKeyboard.hpp"

using namespace input;

void MouseKeyboardHandler::handleInput()
{
    Vector2 mousePos = GetMousePosition();
    _cursorCoords = Utilities::Vector2D(mousePos.x, mousePos.y);

    for (auto bind = _inputBindings.begin(); bind != _inputBindings.end(); bind++) {
        if (IsMouseButtonDown(bind->first) || IsKeyDown(bind->first)) {
            updateState(bind->second, State::PRESSED);
        } else if (isPressed(bind->second)) {
            updateState(bind->second, State::RELEASED);
        } else {
            updateState(bind->second, State::NOTPRESSED);
        }
    }
}
