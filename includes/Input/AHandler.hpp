/*
** EPITECH PROJECT, 2024
** IsoMaker
** File description:
** AHandler
*/

#pragma once

#include <chrono>
#include <unordered_map>

#include "IHandler.hpp"

namespace input {

    using TimePoint = std::chrono::steady_clock::time_point;

    template <typename T>
    class AHandler : public IHandler<T> {
        public:
        protected:
            AHandler(Type type) :
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

            void handleInput() {
                checkHeldState();
            }
            void handleInput(const SDL_Event &event){
                setState(getGenericFromEvent(event), getGenericStateFromEvent(event));
            }
            void checkHeldState() {
                auto now = std::chrono::steady_clock::now();
                for (auto &inputState : _inputStates) {
                    if (inputState.second == State::PRESSED) {
                        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - _holdTimestamps[inputState.first]);
                        if (elapsed >= _holdThreshold) {
                            inputState.second = State::HELD;  // Transition to HELD if the threshold is met
                        }
                    }
                }
            }
        
            void setBinding(T binding, Generic input) {
                _inputBindings[binding] = input;
            }
            void eraseBinding(T binding) {
                _inputBindings.erase(binding);
            }

            void setState(Generic input, State state) {
                if (input == Generic::VOID || (_inputStates[input] == State::PRESSED && state == State::PRESSED)) {
                    return;
                }
                _inputStates[input] = state;

                if (state == State::PRESSED) {
                    _holdTimestamps[input] = std::chrono::steady_clock::now();  // Record the press time
                } else if (state == State::RELEASED) {
                    _holdTimestamps.erase(input);  // Remove the timestamp on release
                }
            }
            std::unordered_map<Generic, State> getStates() const {
                return _inputStates;
            }

            Generic getGenericFromEvent(const SDL_Event &event) const {
                return Generic::VOID;
            }
            State getGenericStateFromEvent(const SDL_Event &event) const {
                return (event.type == SDL_KEYDOWN || event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_CONTROLLERBUTTONDOWN) ? State::PRESSED : State::RELEASED;
            }

            Type _type;
            std::chrono::milliseconds _holdThreshold;
            std::unordered_map<Generic, State> _inputStates;
            std::unordered_map<Generic, TimePoint> _holdTimestamps;
            std::unordered_map<T, Generic> _inputBindings;
        private:
    };
}
