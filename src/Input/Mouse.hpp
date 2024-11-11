/*
** EPITECH PROJECT, 2024
** IsoMaker
** File description:
** Mouse
*/

#pragma once

#include "../../includes/Input/AHandler.hpp"

namespace input {
    class MouseHandler : public AHandler<Uint8> {
        public:
            MouseHandler() : AHandler(Type::MOUSE) {
                inputBindings = {
                    {SDL_BUTTON_LEFT, Generic::SELECT1},
                    {SDL_BUTTON_RIGHT, Generic::SELECT2},
                    {SDL_BUTTON_MIDDLE, Generic::VOID},
                };
            }

            ~MouseHandler() = default;

        private:
            Generic getGenericFromEvent(const SDL_Event &event) const;
            State getGenericStateFromEvent(const SDL_Event &event) const;
    };
}
