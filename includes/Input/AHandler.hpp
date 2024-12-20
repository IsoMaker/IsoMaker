/*
** EPITECH PROJECT, 2024
** IsoMaker
** File description:
** AHandler
*/

#pragma once

/*#include <thread>
#include <chrono>
#include <unordered_map>

#include "IHandler.hpp"

namespace input {

    using TimePoint = std::chrono::steady_clock::time_point;

    template <typename T>
    class AHandler : public IHandler<T> {
        public:
            AHandler(Type type) :
                _running(false),
                _type(type),
                _holdThreshold(std::chrono::milliseconds(200)),
                _inputStates({
                    {Generic::UP, State::RELEASED},
                    {Generic::DOWN, State::RELEASED},
                    {Generic::LEFT, State::RELEASED},
                    {Generic::RIGHT, State::RELEASED},
                    {Generic::INTERACT, State::RELEASED},
                    {Generic::ATTACK, State::RELEASED},
                    {Generic::INVENTORY, State::RELEASED},
                    {Generic::SELECT1, State::RELEASED},
                    {Generic::PAUSE, State::RELEASED},
                }) {};
            virtual ~AHandler() = default;

            State getState(Generic input) const {
                return _inputStates.at(input);
            }
            std::unordered_map<Generic, State> getStates() const {
                return _inputStates;
            }
            std::mutex &getMutex() {
                return _inputMutex;
            }

            void start();

        protected:
            void checkHeldState();
            void updateState(Generic input, State state);

            void handleInput() {
                std::lock_guard<std::mutex> lock(_inputMutex);
                checkHeldState();
            }
            void handleInput(const SDL_Event &event) {
                std::lock_guard<std::mutex> lock(_inputMutex);
                updateState(getGenericFromEvent(event), getGenericStateFromEvent(event));
            }

            void setBinding(T binding, Generic input) {
                _inputBindings[binding] = input;
            }
            void eraseBinding(T binding) {
                _inputBindings.erase(binding);
            }

            Generic getGenericFromEvent(const SDL_Event &event) const {
                return Generic::VOID;
            }
            State getGenericStateFromEvent(const SDL_Event &event) const {
                return (event.type == SDL_KEYDOWN || event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_CONTROLLERBUTTONDOWN) ? State::PRESSED : State::RELEASED;
            }

            bool _running;
            Type _type;
            std::chrono::milliseconds _holdThreshold;
            std::unordered_map<Generic, State> _inputStates;
            std::unordered_map<Generic, TimePoint> _holdTimestamps;
            std::unordered_map<T, Generic> _inputBindings;
            std::mutex _inputMutex;
        private:
    };
}
*/