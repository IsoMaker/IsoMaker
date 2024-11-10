/*
** EPITECH PROJECT, 2024
** IsoMaker
** File description:
** Controller
*/

#pragma once

#include "Handler.hpp"

namespace input {
    class ControllerHandler : public AHandler {
        public:
            ControllerHandler() : AHandler(Type::GAMEPAD) {
                controllerBindings = {
                    {SDL_CONTROLLER_BUTTON_DPAD_UP, Generic::UP},
                    {SDL_CONTROLLER_BUTTON_DPAD_DOWN, Generic::DOWN},
                    {SDL_CONTROLLER_BUTTON_DPAD_LEFT, Generic::LEFT},
                    {SDL_CONTROLLER_BUTTON_DPAD_RIGHT, Generic::RIGHT},
                    {SDL_CONTROLLER_BUTTON_A, Generic::INTERACT},
                    {SDL_CONTROLLER_BUTTON_B, Generic::ATTACK},
                    {SDL_CONTROLLER_BUTTON_X, Generic::INVENTORY},
                    {SDL_CONTROLLER_BUTTON_START, Generic::PAUSE}
                };
            };

        private:
            std::unordered_map<Uint8, Generic> controllerBindings;

            Generic getGenericFromEvent(const SDL_Event &event) const {
                auto it = controllerBindings.find(event.cbutton.button);
                return (it != controllerBindings.end()) ? it->second : Generic::VOID;
            }

            State getState(const SDL_Event &event) const {
                return (event.cbutton.state == SDL_PRESSED) ? State::PRESSED : State::RELEASED;
            }
    };
}