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
    std::shared_ptr<Render::Camera> camera = std::make_shared<Render::Camera>();
    std::shared_ptr<Render::Window> window = std::make_shared<Render::Window>();
    MainUI mainUI(camera, window);
    input::MouseKeyboardHandler inputHandler;

    std::thread mouseKeyboardThread(mouseLoop, std::ref(inputHandler), std::ref(running));

    mainUI.loop(inputHandler);

    running = false;
    mouseKeyboardThread.join();
}
