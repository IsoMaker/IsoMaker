#include "Game.hpp"
#include "Character.hpp"

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
    input::MouseKeyboardHandler inputHandler;
    Render::Camera camera;
    Render::Window window;
    bool running = true;
    Game game(window, camera);

    std::thread mouseKeyboardThread(mouseLoop, std::ref(inputHandler), std::ref(running));

    game.loop(inputHandler);

    running = false;
    mouseKeyboardThread.join();
    return 0;
}
