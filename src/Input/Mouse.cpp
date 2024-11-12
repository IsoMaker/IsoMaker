/*
** EPITECH PROJECT, 2024
** IsoMaker
** File description:
** Mouse
*/

#include "Mouse.hpp"

using namespace input;

Generic MouseHandler::getGenericFromEvent(const SDL_Event &event) const {
    if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
        auto it = _inputBindings.find(event.button.button);
        return (it != _inputBindings.end()) ? it->second : Generic::VOID;
    }
    return Generic::VOID;
}

State MouseHandler::getGenericStateFromEvent(const SDL_Event &event) const {
    return (event.type == SDL_MOUSEBUTTONDOWN) ? State::PRESSED : State::RELEASED;
}
