/*
** EPITECH PROJECT, 2024
** IsoMaker
** File description:
** Controller
*/

#include "Controller.hpp"

using namespace input;

Generic ControllerHandler::getGenericFromEvent(const SDL_Event &event) const {
    if (event.type == SDL_CONTROLLERBUTTONDOWN || event.type == SDL_CONTROLLERBUTTONUP) {
        auto it = _inputBindings.find(event.cbutton.button);
        return (it != _inputBindings.end()) ? it->second : Generic::VOID;
    }
    return Generic::VOID;
}

State ControllerHandler::getGenericStateFromEvent(const SDL_Event &event) const {
    return (event.type == SDL_CONTROLLERBUTTONDOWN) ? State::PRESSED : State::RELEASED;
}
