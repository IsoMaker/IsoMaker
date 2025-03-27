#define RAYGUI_IMPLEMENTATION

#include "Input/MouseKeyboard.hpp"
#include "Input/Gamepad.hpp"
#include "MainUI/MainUI.hpp"

void mouseLoop(input::MouseKeyboardHandler &inputHandler, bool &running)
{
    while (running) {
        std::lock_guard<std::mutex> lock(inputHandler.getMutex());
        inputHandler.loop();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

int main()
{
    bool running = true;
    MainUI mainUI;
    input::MouseKeyboardHandler inputHandler;

    std::thread mouseThread(mouseLoop, std::ref(mouseHandler), std::ref(running));
    std::thread keyboardThread(keyboardLoop, std::ref(keyboardHandler), std::ref(running));

    mainUI.loop(mouseHandler, keyboardHandler);

    running = false;
    mouseThread.join();
    keyboardThread.join();
}
