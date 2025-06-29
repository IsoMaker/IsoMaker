#include "Game.hpp"

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
    std::shared_ptr<Render::Window> window = std::make_shared<Render::Window>();
    std::shared_ptr<Render::Camera> camera = std::make_shared<Render::Camera>();
    Game game(window, camera);
    bool running = true;

    std::thread mouseKeyboardThread(mouseLoop, std::ref(inputHandler), std::ref(running));

    game.loop(inputHandler);

    running = false;
    mouseKeyboardThread.join();
    return 0;
}
