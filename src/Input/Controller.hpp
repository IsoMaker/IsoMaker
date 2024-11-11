/*
** EPITECH PROJECT, 2024
** IsoMaker
** File description:
** Controller
*/

#pragma once

#include "../../includes/Input/AHandler.hpp"

namespace input {
    class ControllerHandler : public AHandler<Uint8> {
        public:
            ControllerHandler() : AHandler(Type::GAMEPAD) {
                inputBindings = {
                    {SDL_CONTROLLER_BUTTON_DPAD_UP, Generic::UP},
                    {SDL_CONTROLLER_BUTTON_DPAD_DOWN, Generic::DOWN},
                    {SDL_CONTROLLER_BUTTON_DPAD_LEFT, Generic::LEFT},
                    {SDL_CONTROLLER_BUTTON_DPAD_RIGHT, Generic::RIGHT},
                    {SDL_CONTROLLER_BUTTON_A, Generic::INTERACT},
                    {SDL_CONTROLLER_BUTTON_B, Generic::ATTACK},
                    {SDL_CONTROLLER_BUTTON_X, Generic::INVENTORY},
                    {SDL_CONTROLLER_BUTTON_START, Generic::PAUSE},
                };
            };
            ~ControllerHandler() = default;

        private:
            Generic getGenericFromEvent(const SDL_Event &event) const;
            State getGenericStateFromEvent(const SDL_Event &event) const;
    };
}
