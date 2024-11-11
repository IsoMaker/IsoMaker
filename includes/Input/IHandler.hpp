/*
** EPITECH PROJECT, 2024
** IsoMaker
** File description:
** IHandler
*/

#pragma once

#include <SDL2/SDL.h> 

namespace input {

    enum class Type {
        KEYBOARD,
        MOUSE,
        GAMEPAD
    };

    enum class State {
        PRESSED,
        RELEASED,
        HELD
    };

    enum class Generic {
        UP,
        DOWN,
        LEFT,
        RIGHT,
        INTERACT,
        ATTACK,
        INVENTORY,
        SELECT,
        PAUSE,
        VOID
    };

    template <typename T>
    class IHandler {
        public:
            IHandler() = default;
            virtual ~IHandler() = default;

            virtual void handleInput() = 0;
            virtual void handleInput(const SDL_Event &event) = 0;
            virtual void checkHeldState() = 0;

            virtual void setBinding(T binding, Generic input) = 0;
            virtual void eraseBinding(T binding) = 0;

            virtual void setState(Generic input, State state) = 0;
            virtual std::unordered_map<Generic, State> getStates() const = 0;

            virtual Generic getGenericFromEvent(const SDL_Event &event) const = 0;
            virtual State getGenericStateFromEvent(const SDL_Event &event) const = 0;
        protected:
        private:
    };
}
