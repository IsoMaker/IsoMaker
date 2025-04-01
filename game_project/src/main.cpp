#include "Game.hpp"

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
    input::MouseHandler mouseHandler;
    input::KeyboardHandler keyboardHandler;
    Render::Camera camera;
    Render::Window window;
    bool running = true;
    Game game(window, camera);

    std::thread mouseThread(mouseLoop, std::ref(mouseHandler), std::ref(running));
    std::thread keyboardThread(keyboardLoop, std::ref(keyboardHandler), std::ref(running));

    game.loop(mouseHandler, keyboardHandler);

    running = false;
    mouseThread.join();
    keyboardThread.join();
    return 0;
}

