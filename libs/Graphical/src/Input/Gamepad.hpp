/*
** EPITECH PROJECT, 2024
** IsoMaker
** File description:
** Controller
*/

#pragma once

#include "../../includes/Input/AHandler.hpp"

namespace input
{
    class GamepadHandler : public AHandler<Uint8>
    {
        public:
            GamepadHandler() : AHandler(Type::GAMEPAD)
            {
                _inputBindings = {
                    {SDL_CONTROLLER_BUTTON_DPAD_UP, Generic::UP},
                    {SDL_CONTROLLER_BUTTON_DPAD_DOWN, Generic::DOWN},
                    {SDL_CONTROLLER_BUTTON_DPAD_LEFT, Generic::LEFT},
                    {SDL_CONTROLLER_BUTTON_DPAD_RIGHT, Generic::RIGHT},
                    {SDL_CONTROLLER_BUTTON_A, Generic::ATTACK},
                    {SDL_CONTROLLER_BUTTON_B, Generic::INTERACT1},
                    {SDL_CONTROLLER_BUTTON_X, Generic::INTERACT2},
                    {SDL_CONTROLLER_BUTTON_Y, Generic::INTERACT3},
                    {SDL_CONTROLLER_BUTTON_START, Generic::PAUSE},
                    {SDL_CONTROLLER_BUTTON_BACK, Generic::INVENTORY},
                    {SDL_CONTROLLER_BUTTON_LEFTSHOULDER, Generic::VOID},
                    {SDL_CONTROLLER_BUTTON_RIGHTSHOULDER, Generic::VOID},
                };
            };

            ~GamepadHandler() = default;

            std::unordered_map<Uint8, Generic> getBindings() const { return _inputBindings; }
        protected:
        private:
            void handleInput();
    };
}
