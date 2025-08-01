#pragma once
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

namespace input
{
    using TimePoint = std::chrono::steady_clock::time_point;

    template <typename T>
    class AHandler : public IHandler<T>
    {
        public:
            AHandler(Type type) :
                _running(false),
                _type(type),
                _inputStates({
                    {Generic::UP, State::NOTPRESSED},
                    {Generic::DOWN, State::NOTPRESSED},
                    {Generic::LEFT, State::NOTPRESSED},
                    {Generic::RIGHT, State::NOTPRESSED},
                    {Generic::INTERACT1, State::NOTPRESSED},
                    {Generic::INTERACT2, State::NOTPRESSED},
                    {Generic::INTERACT3, State::NOTPRESSED},
                    {Generic::INTERACT4, State::NOTPRESSED},
                    {Generic::ATTACK, State::NOTPRESSED},
                    {Generic::INVENTORY, State::NOTPRESSED},
                    {Generic::PAUSE, State::NOTPRESSED},
                    {Generic::ENTER, State::NOTPRESSED},
                    {Generic::SELECT1, State::NOTPRESSED},
                    {Generic::SELECT2, State::NOTPRESSED},
                    {Generic::SELECT3, State::NOTPRESSED},
                    {Generic::SELECT4, State::NOTPRESSED},
                    {Generic::VOID, State::NOTPRESSED}, }),
                _holdThreshold(std::chrono::milliseconds(300)),
                _holdTimestamps({
                    {Generic::UP, TimePoint()},
                    {Generic::DOWN, TimePoint()},
                    {Generic::LEFT, TimePoint()},
                    {Generic::RIGHT, TimePoint()},
                    {Generic::INTERACT1, TimePoint()},
                    {Generic::INTERACT2, TimePoint()},
                    {Generic::INTERACT3, TimePoint()},
                    {Generic::INTERACT4, TimePoint()},
                    {Generic::ATTACK, TimePoint()},
                    {Generic::INVENTORY, TimePoint()},
                    {Generic::PAUSE, TimePoint()},
                    {Generic::ENTER, TimePoint()},
                    {Generic::SELECT1, TimePoint()},
                    {Generic::SELECT2, TimePoint()},
                    {Generic::SELECT3, TimePoint()},
                    {Generic::SELECT4, TimePoint()},
                    {Generic::VOID, TimePoint()}, }),
                _inputMutex() {}

            virtual ~AHandler() = default;

            Type getType() const { return _type; }

            std::unordered_map<T, Generic> getBindings() const { return _inputBindings; }

            bool isNotPressed(Generic input) const { return _inputStates.at(input) == State::NOTPRESSED; }
            bool isPressed(Generic input) const { return _inputStates.at(input) == State::PRESSED; }
            bool isHeld(Generic input) const { return _inputStates.at(input) == State::HELD; }
            bool isReleased(Generic input) const { return _inputStates.at(input) == State::RELEASED; }

            State getState(Generic input) const { return _inputStates.at(input); }
            std::unordered_map<Generic, State> getStates() const { return _inputStates; }

            Utilities::Vector2D getCursorCoords() const { return _cursorCoords; }

            std::mutex &getMutex() { return _inputMutex; }

            void loop()
            {
                if (WindowShouldClose()) {
                    return;
                }
                handleInput();
                checkHeldState(); // check held states
            }

        protected:
            void handleInput()
            {
                std::lock_guard<std::mutex> lock(_inputMutex);
            }

            void checkHeldState()
            {
                TimePoint now = std::chrono::steady_clock::now();
                for (auto &inputState : _inputStates) {
                    if (isPressed(inputState.first) && _holdTimestamps.find(inputState.first) != _holdTimestamps.end()) {
                        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - _holdTimestamps.at(inputState.first));
                        if (elapsed >= _holdThreshold) {
                            updateState(inputState.first, State::HELD);  // transition to HELD if the threshold is met
                        }
                    }
                }
            }

            void updateState(Generic input, State state)
            {
                if (input == Generic::VOID) {
                    return;
                } else if (state == State::PRESSED && isNotPressed(input)) {
                    _holdTimestamps.at(input) = std::chrono::steady_clock::now();
                }
                _inputStates.at(input) = state;
            }

            void setBinding(T binding, Generic input)
            {
                std::lock_guard<std::mutex> lock(_inputMutex);
                std::cout << "Binding lock" << std::endl;
                _inputBindings[binding] = input;
            }
            void clearBinding(T binding)
            {
                std::lock_guard<std::mutex> lock(_inputMutex);
                _inputBindings[binding] = Generic::VOID;
            }
            void eraseBinding(T binding)
            {
                std::lock_guard<std::mutex> lock(_inputMutex);
                _inputBindings.erase(binding);
            }

            bool _running;
            Type _type;
            std::unordered_map<T, Generic> _inputBindings;
            std::unordered_map<Generic, State> _inputStates;
            std::chrono::milliseconds _holdThreshold;
            std::unordered_map<Generic, TimePoint> _holdTimestamps;
            Utilities::Vector2D _cursorCoords;
            std::mutex _inputMutex;
        private:
    };
}
