/*
** EPITECH PROJECT, 2024
** IsoMaker
** File description:
** AHandler
*/

#pragma once

#include <thread>
#include <chrono>
#include <unordered_map>
#include <iostream>
#include "IHandler.hpp"

namespace input {

    using TimePoint = std::chrono::steady_clock::time_point;

    template <typename T>
    class AHandler : public IHandler<T> {
        public:
            AHandler(Type type) :
                _running(false),
                _type(type),
                _holdThreshold(std::chrono::milliseconds(300)),
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
                }) {}
            virtual ~AHandler() = default;

            State getState(Generic input) const
            {
                return _inputStates.at(input);
            }
            bool isPressed(Generic input) const
            {
                return _inputStates.at(input) == State::PRESSED;
            }
            bool isHeld(Generic input) const
            {
                return _inputStates.at(input) == State::HELD;
            }
            bool isReleased(Generic input) const
            {
                return _inputStates.at(input) == State::RELEASED;
            }
            std::unordered_map<Generic, State> getStates() const
            {
                return _inputStates;
            }
            std::mutex &getMutex()
            {
                return _inputMutex;
            }

            void loop()
            {
                if (WindowShouldClose()) {
                    return;
                }
                handleInput();
                checkHeldState(); // check held states
            }

        protected:
            void checkHeldState()
            {
                auto now = std::chrono::steady_clock::now();
                for (auto &inputState : _inputStates) {
                    if (inputState.second == State::PRESSED) {
                        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - _holdTimestamps[inputState.first]);
                        if (elapsed >= _holdThreshold) {
                            inputState.second = State::HELD;  // transition to HELD if the threshold is met
                        }
                    }
                }
            }

            void updateState(Generic input, State state)
            {
                if (input == Generic::VOID || (_inputStates[input] == State::PRESSED && state == State::PRESSED)) {
                    return;
                }
                _inputStates[input] = state;

                if (state == State::PRESSED) {
                    _holdTimestamps[input] = std::chrono::steady_clock::now();  // record the press time
                } else if (state == State::RELEASED) {
                    _holdTimestamps.erase(input);  // remove the timestamp on release
                }
            }

            void handleInput()
            {
                std::lock_guard<std::mutex> lock(_inputMutex);
            }
            void handleInput(const SDL_Event &event)
            {
                std::cout << "Generic input" << std::endl;
                std::lock_guard<std::mutex> lock(_inputMutex);
            }

            void setBinding(T binding, Generic input)
            {
                std::lock_guard<std::mutex> lock(_inputMutex);
                std::cout << "Binding lock" << std::endl;
                _inputBindings[binding] = input;
            }
            void eraseBinding(T binding)
            {
                std::lock_guard<std::mutex> lock(_inputMutex);
                _inputBindings.erase(binding);
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

