/*
** EPITECH PROJECT, 2024
** IsoMaker
** File description:
** Mouse
*/

#pragma once

#include "../../includes/Input/AHandler.hpp"
#include "../Utilities/Vector.hpp"

namespace input
{
    class MouseKeyboardHandler : public AHandler<int>
    {
        public:
            MouseKeyboardHandler() : AHandler(Type::KEYBOARDMOUSE)
            {
                _inputBindings = {
                    {MOUSE_BUTTON_LEFT, Generic::SELECT1},
                    {MOUSE_BUTTON_RIGHT, Generic::SELECT2},
                    {MOUSE_BUTTON_MIDDLE, Generic::VOID},
                    {KEY_UP, Generic::UP},
                    {KEY_DOWN, Generic::DOWN},
                    {KEY_LEFT, Generic::LEFT},
                    {KEY_RIGHT, Generic::RIGHT},
                    {KEY_ENTER, Generic::INTERACT1},
                    {KEY_Q, Generic::INTERACT2},
                    {KEY_E, Generic::INTERACT3},
                    {KEY_SPACE, Generic::ATTACK},
                    {KEY_TAB, Generic::INVENTORY},
                    {KEY_ESCAPE, Generic::PAUSE},
                };
            }

            ~MouseKeyboardHandler() = default;

            std::unordered_map<int, Generic> getBindings() const { return _inputBindings; }
        protected:
        private:
            void handleInput();
    };
}
