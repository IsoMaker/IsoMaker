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
                }) {}
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

            void start() {
                _running = true;
                SDL_Event event;

                while (_running) {
                    while (SDL_PollEvent(&event)) {
                        if (event.type == SDL_QUIT) {
                            _running = false;
                            break;
                        }
                        handleInput(event); // handle input events
                    }

                    handleInput(); // check held states
                    std::this_thread::sleep_for(std::chrono::milliseconds(16)); // ~60 FPS
                }
            }

        protected:
            void checkHeldState() {
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

            void updateState(Generic input, State state) {
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

            void handleInput() {
                std::lock_guard<std::mutex> lock(_inputMutex);
                checkHeldState();
            }
            void handleInput(const SDL_Event &event) {
                std::lock_guard<std::mutex> lock(_inputMutex);
                updateState(getGenericFromEvent(event), getGenericStateFromEvent(event));
            }

            void setBinding(T binding, Generic input) {
                std::lock_guard<std::mutex> lock(_inputMutex);
                _inputBindings[binding] = input;
            }
            void eraseBinding(T binding) {
                std::lock_guard<std::mutex> lock(_inputMutex);
                _inputBindings.erase(binding);
            }

            virtual Generic getGenericFromEvent(const SDL_Event &event) const = 0;
            virtual State getGenericStateFromEvent(const SDL_Event &event) const = 0;

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

