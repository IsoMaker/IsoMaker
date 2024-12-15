/*
** EPITECH PROJECT, 2024
** Visual Studio Live Share (Workspace)
** File description:
** AHandler
*/

#include "../../includes/Input/AHandler.hpp"

using namespace input;

void AHandler::start()
{
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

void AHandler::checkHeldState()
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

void AHandler::updateState(Generic input, State state)
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
