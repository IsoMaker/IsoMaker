#define RAYGUI_IMPLEMENTATION

#include "Input/Mouse.hpp"
#include "Input/Keyboard.hpp"
#include "MainUI/MainUI.hpp"

void mouseLoop(input::MouseHandler &mouseHandler, bool &running)
{
    while (running) {
        std::lock_guard<std::mutex> lock(mouseHandler.getMutex());
        mouseHandler.loop();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

void keyboardLoop(input::KeyboardHandler &keyboardHandler, bool &running)
{
    while (running) {
        std::lock_guard<std::mutex> lock(keyboardHandler.getMutex());
        keyboardHandler.loop();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

int main()
{
    bool running = true;
    MainUI mainUI;
    input::MouseHandler mouseHandler;
    input::KeyboardHandler keyboardHandler;

    std::thread mouseThread(mouseLoop, std::ref(mouseHandler), std::ref(running));
    // std::thread keyboardThread(keyboardLoop, std::ref(keyboardHandler), std::ref(running));

    mainUI.loop(mouseHandler);

    running = false;
    mouseThread.join();
}
