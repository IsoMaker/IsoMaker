/*
** EPITECH PROJECT, 2024
** IsoMaker
** File description:
** Keyboard
*/

#pragma once

#include "Handler.hpp"

namespace input {
    class KeyboardHandler : public AHandler {
        public:
            KeyboardHandler() : AHandler(Type::KEYBOARD) {
                keyboardBindings = {
                    {SDLK_UP, Generic::UP},
                    {SDLK_DOWN, Generic::DOWN},
                    {SDLK_LEFT, Generic::LEFT},
                    {SDLK_RIGHT, Generic::RIGHT},
                    {SDLK_e, Generic::INTERACT},
                    {SDLK_SPACE, Generic::ATTACK},
                    {SDLK_i, Generic::INVENTORY},
                    {SDLK_RETURN, Generic::SELECT},
                    {SDLK_ESCAPE, Generic::PAUSE}
                };
            };

        private:
            std::unordered_map<SDL_Keycode, Generic> keyboardBindings;

            Generic getGenericFromEvent(const SDL_Event &event) const {
                auto it = keyboardBindings.find(event.key.keysym.sym);
                return (it != keyboardBindings.end()) ? it->second : Generic::VOID;
            }

            State getState(const SDL_Event &event) const {
                return (event.type == SDL_KEYDOWN) ? State::PRESSED : State::RELEASED;
            }
    };
}