/*
** EPITECH PROJECT, 2024
** IsoMaker
** File description:
** IHandler
*/

#pragma once

/*#include <SDL2/SDL.h>
#include <mutex>

#include "InputTypes.hpp"

namespace input {
    template <typename T>
    class IHandler {
        public:
            IHandler() = default;
            virtual ~IHandler() = default;

            virtual void start() = 0;

            virtual State getState(Generic input) const = 0;
            virtual std::unordered_map<Generic, State> getStates() const = 0;

            virtual void checkHeldState() = 0;
            virtual void updateState(Generic input, State state) = 0;

            virtual void handleInput() = 0;
            virtual void handleInput(const SDL_Event &event) = 0;

            virtual void setBinding(T binding, Generic input) = 0;
            virtual void eraseBinding(T binding) = 0;

            virtual Generic getGenericFromEvent(const SDL_Event &event) const = 0;
            virtual State getGenericStateFromEvent(const SDL_Event &event) const = 0;

            virtual std::mutex &getMutex() = 0;
        protected:
        private:
    };
}
*/