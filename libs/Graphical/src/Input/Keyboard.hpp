/*
** EPITECH PROJECT, 2024
** IsoMaker
** File description:
** Keyboard
*/

#pragma once

#include "../../includes/Input/AHandler.hpp"
#include "../Utilities/Vector.hpp"

namespace input {
    class KeyboardHandler : public AHandler<int> {
        public:
            KeyboardHandler() : AHandler(Type::KEYBOARD)
            {
                _inputBindings = {
                    {KEY_W, Generic::UP},
                    {KEY_S, Generic::DOWN},
                    {KEY_A, Generic::LEFT},
                    {KEY_D, Generic::RIGHT},
                    {KEY_ENTER, Generic::INTERACT},
                    {KEY_SPACE, Generic::ATTACK},
                    {KEY_TAB, Generic::INVENTORY},
                    {KEY_ESCAPE, Generic::PAUSE},
                };
            };
            ~KeyboardHandler() = default;

        private:

            Generic getGenericFromInput(int) const;
            void handleInput();
    };
}
