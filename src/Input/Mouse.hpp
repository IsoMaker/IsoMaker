/*
** EPITECH PROJECT, 2024
** IsoMaker
** File description:
** Mouse
*/

#pragma once

#include "../../includes/Input/AHandler.hpp"
#include "../Utilities/Vector.hpp"

namespace input {
    class MouseHandler : public AHandler<int> {
        public:
            MouseHandler() : AHandler(Type::MOUSE)
            {
                _inputBindings = {
                    {MOUSE_BUTTON_LEFT, Generic::SELECT1},
                    {MOUSE_BUTTON_RIGHT, Generic::SELECT2},
                    {MOUSE_BUTTON_MIDDLE, Generic::VOID},
                };
            }

            ~MouseHandler() = default;

            Utilities::Vector2D getMouseCoords() const { return _mouseCoords; }

        private:
            void handleInput();

            Utilities::Vector2D _mouseCoords;
    };
}
