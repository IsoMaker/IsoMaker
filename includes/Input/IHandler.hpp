/*
** EPITECH PROJECT, 2024
** IsoMaker
** File description:
** IHandler
*/

#pragma once

#include <SDL2/SDL.h> 

#include "../Game/Effect.hpp"

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

    class IHandler {
        public:
            IHandler() = default;
            virtual ~IHandler() = default;

            virtual void setBinding(Generic input, game::Effect effect) = 0;
            virtual void eraseBinding(Generic input) = 0;

            virtual void handleInput() = 0;
            virtual void handleInput(const SDL_Event &event) = 0;
            virtual void handleInput(const SDL_Event &event, game::Effect effect) = 0;
            virtual void checkHeldState() = 0;

            virtual Generic getGenericFromEvent(const SDL_Event &event) const = 0;
            virtual State getState(const SDL_Event &event) const = 0;
            virtual void setState(Generic input, State state) = 0;
            virtual game::Effect getEffect(const SDL_Event &event) const = 0;

        protected:
        private:
    };
}
