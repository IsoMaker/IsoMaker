/*
** EPITECH PROJECT, 2024
** IsoMaker
** File description:
** Keyboard
*/

#pragma once

#include "../../includes/Input/AHandler.hpp"

namespace input {
    class KeyboardHandler : public AHandler<SDL_Keycode> {
        public:
            KeyboardHandler() : AHandler(Type::KEYBOARD) {
                _inputBindings = {
                    {SDLK_w, Generic::UP},
                    {SDLK_s, Generic::DOWN},
                    {SDLK_a, Generic::LEFT},
                    {SDLK_d, Generic::RIGHT},
                    {SDLK_RETURN, Generic::INTERACT},
                    {SDLK_SPACE, Generic::ATTACK},
                    {SDLK_TAB, Generic::INVENTORY},
                    {SDLK_ESCAPE, Generic::PAUSE},
                    {SDLK_RETURN, Generic::ENTER},
                };
            };
            ~KeyboardHandler() = default;

        private:
            Generic getGenericFromEvent(const SDL_Event &event) const;
            State getGenericStateFromEvent(const SDL_Event &event) const;
    };
}
