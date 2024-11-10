/*
** EPITECH PROJECT, 2024
** IsoMaker
** File description:
** InputHandler
*/

#pragma once

#include <chrono>
#include <unordered_map>
#include <SDL2/SDL.h> 

#include "../Game/Effect.hpp"

namespace input {

    using TimePoint = std::chrono::steady_clock::time_point;

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
            virtual void replaceBinding(Generic input, game::Effect effect) = 0;

            virtual void handleInput() = 0;
            virtual void handleInput(const SDL_Event &event) = 0;
            virtual void handleInput(const SDL_Event &event, game::Effect effect) = 0;

            virtual game::Effect mapInputToEffect(const SDL_Event &event) const = 0;

        protected:
            virtual Generic getGenericFromEvent(const SDL_Event &event) const = 0;
            virtual game::Effect getEffect(Generic input) const = 0;
            virtual State getState(const SDL_Event &event) const = 0;
            virtual void setState(Generic input, State state) = 0;

        private:
    };

    class AHandler : public IHandler {
        public:
            AHandler(Type type) {
                this->type = type;
                this->inputEffects = {
                    {Generic::UP, game::Effect::UP},
                    {Generic::DOWN, game::Effect::DOWN},
                    {Generic::LEFT, game::Effect::LEFT},
                    {Generic::RIGHT, game::Effect::RIGHT},
                    {Generic::INTERACT, game::Effect::INTERACT},
                    {Generic::ATTACK, game::Effect::ATTACK},
                    {Generic::INVENTORY, game::Effect::INVENTORY},
                    {Generic::SELECT, game::Effect::SELECT},
                    {Generic::PAUSE, game::Effect::PAUSE}
                };
                this->inputStates = {
                    {Generic::UP, State::RELEASED},
                    {Generic::DOWN, State::RELEASED},
                    {Generic::LEFT, State::RELEASED},
                    {Generic::RIGHT, State::RELEASED},
                    {Generic::INTERACT, State::RELEASED},
                    {Generic::ATTACK, State::RELEASED},
                    {Generic::INVENTORY, State::RELEASED},
                    {Generic::SELECT, State::RELEASED},
                    {Generic::PAUSE, State::RELEASED}
                };
            }
            ~AHandler() = default;

            void setBinding(Generic input, game::Effect effect) {
                inputEffects[input] = effect;
            }
            void eraseBinding(Generic input) {
                inputEffects.erase(input);
            }
            void replaceBinding(Generic input, game::Effect effect) {
                eraseBinding(input);
                setBinding(input, effect);
            }

            void handleInput() override {
                checkHeldState();
            }
            void handleInput(const SDL_Event &event) {
                setState(getGenericFromEvent(event), getState(event));
            }

            game::Effect mapInputToEffect(const SDL_Event &event) const override {
                Generic genericInput = getGenericFromEvent(event);
                return getEffect(genericInput);
            }

        protected:
            Type type;
            std::unordered_map<Generic, game::Effect> inputEffects;
            std::unordered_map<Generic, State> inputStates;
            std::unordered_map<Generic, TimePoint> holdTimestamps;

        private:
            int holdThreshold = 200;  // 200ms

            void checkHeldState() {
                auto now = std::chrono::steady_clock::now();
                for (auto &[input, state] : inputStates) {
                    if (state == State::PRESSED) {
                        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - holdTimestamps[input]);
                        if (elapsed >= std::chrono::milliseconds(holdThreshold)) {
                            state = State::HELD;  // Transition to HELD if the threshold is met
                        }
                    }
                }
            }

            game::Effect getEffect(Generic input) const {
                auto it = inputEffects.find(input);
                return (it != inputEffects.end()) ? it->second : game::Effect::VOID; // Default or no-op effect
            }

            void setState(Generic input, State state) {
                if (input == Generic::VOID || (inputStates[input] == State::PRESSED && state == State::PRESSED)) {
                    return;
                }
                inputStates[input] = state;

                if (state == State::PRESSED) {
                    holdTimestamps[input] = std::chrono::steady_clock::now();  // Record the press time
                } else if (state == State::RELEASED) {
                    holdTimestamps.erase(input);  // Remove the timestamp on release
                }
            }
    };
}
