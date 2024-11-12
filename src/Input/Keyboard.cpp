/*
** EPITECH PROJECT, 2024
** IsoMaker
** File description:
** Keyboard
*/

#include "Keyboard.hpp"

using namespace input;

Generic KeyboardHandler::getGenericFromEvent(const SDL_Event &event) const {
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        auto it = _inputBindings.find(event.key.keysym.sym);
        return (it != _inputBindings.end()) ? it->second : Generic::VOID;
    }
    return Generic::VOID;
}

State KeyboardHandler::getGenericStateFromEvent(const SDL_Event &event) const {
    return (event.type == SDL_KEYDOWN) ? State::PRESSED : State::RELEASED;
}
