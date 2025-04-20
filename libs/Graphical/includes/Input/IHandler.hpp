/*
** EPITECH PROJECT, 2024
** IsoMaker
** File description:
** IHandler
*/

#pragma once

#include <SDL2/SDL.h>
#include <mutex>

#include <raylib.h>
#include "InputTypes.hpp"
#include "../../src/Utilities/Vector.hpp"

namespace input
{

    class IHandlerBase
    {
        public:
            IHandlerBase() = default;
            virtual ~IHandlerBase() = default;

            virtual Type getType() const = 0;

            virtual bool isNotPressed(Generic input) const = 0;
            virtual bool isPressed(Generic input) const = 0;
            virtual bool isHeld(Generic input) const = 0;
            virtual bool isReleased(Generic input) const = 0;

            virtual State getState(Generic input) const = 0;
            virtual std::unordered_map<Generic, State> getStates() const = 0;

            virtual Utilities::Vector2D getCursorCoords() const = 0;

            virtual std::mutex &getMutex() = 0;

            virtual void loop() = 0;

            virtual void checkHeldState() = 0;
            virtual void updateState(Generic input, State state) = 0;

            virtual void handleInput() = 0;

        protected:
        private:
    };

    template <typename T>
    class IHandler : public IHandlerBase
    {
        public:
            IHandler() = default;
            virtual ~IHandler() = default;

            virtual void setBinding(T binding, Generic input) = 0;
            virtual void eraseBinding(T binding) = 0;

        protected:
        private:
    };
}
