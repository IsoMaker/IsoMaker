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
    class AHandler : public IHandler {
        public:
        protected:
            AHandler(Type type) :
                type(type),
                holdThreshold(std::chrono::milliseconds(200)),
                inputEffects({
                    {Generic::UP, game::Effect::UP},
                    {Generic::DOWN, game::Effect::DOWN},
                    {Generic::LEFT, game::Effect::LEFT},
                    {Generic::RIGHT, game::Effect::RIGHT},
                    {Generic::INTERACT, game::Effect::INTERACT},
                    {Generic::ATTACK, game::Effect::ATTACK},
                    {Generic::INVENTORY, game::Effect::INVENTORY},
                    {Generic::SELECT, game::Effect::SELECT},
                    {Generic::PAUSE, game::Effect::PAUSE}
                }),
                inputStates({
                    {Generic::UP, State::RELEASED},
                    {Generic::DOWN, State::RELEASED},
                    {Generic::LEFT, State::RELEASED},
                    {Generic::RIGHT, State::RELEASED},
                    {Generic::INTERACT, State::RELEASED},
                    {Generic::ATTACK, State::RELEASED},
                    {Generic::INVENTORY, State::RELEASED},
                    {Generic::SELECT, State::RELEASED},
                    {Generic::PAUSE, State::RELEASED}
                }) {};
            virtual ~AHandler() = default;

            void setBinding(Generic input, game::Effect effect) override {
                inputEffects[input] = effect;
            }
            void eraseBinding(Generic input) override {
                inputEffects.erase(input);
            }

            void handleInput() override {
                checkHeldState();
            }
            void handleInput(const SDL_Event &event) override {
                setState(getGenericFromEvent(event), getState(event));
            }
            void checkHeldState() override {
                auto now = std::chrono::steady_clock::now();
                for (auto &[input, state] : inputStates) {
                    if (state == State::PRESSED) {
                        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - holdTimestamps[input]);
                        if (elapsed >= holdThreshold) {
                            state = State::HELD;  // Transition to HELD if the threshold is met
                        }
                    }
                }
            }

            void setState(Generic input, State state) override {
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
            game::Effect getEffect(const SDL_Event &event) const override {
                Generic genericInput = getGenericFromEvent(event);
                auto it = inputEffects.find(genericInput);
                return (it != inputEffects.end()) ? it->second : game::Effect::VOID;
            }

            Type type;
            std::chrono::milliseconds holdThreshold;
            std::unordered_map<Generic, game::Effect> inputEffects;
            std::unordered_map<Generic, State> inputStates;
            std::unordered_map<Generic, TimePoint> holdTimestamps;
            std::unordered_map<T, Generic> inputBindings;
        private:
    };
}
