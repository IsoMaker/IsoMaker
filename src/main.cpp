#define RAYGUI_IMPLEMENTATION

#include "Input/Mouse.hpp"
#include "MainUI/MainUI.hpp"

void inputLoop(input::MouseHandler &mouseHandler) {
    mouseHandler.start();
}

void userInterfaceLoop(MainUI &mainUI, input::MouseHandler &mouseHandler) {
    mainUI.loop(mouseHandler);
}

int main()
{
    if (SDL_Init(SDL_INIT_EVENTS) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    MainUI mainUI;
    input::MouseHandler mouseHandler;

    std::thread uiThread(userInterfaceLoop, std::ref(mainUI), std::ref(mouseHandler));
    std::thread inputThread(inputLoop, std::ref(mouseHandler));

    uiThread.join();
    inputThread.join();

    SDL_Quit();
}
