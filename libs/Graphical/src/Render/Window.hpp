#pragma once

#include "../Utilities/Vector.hpp"
#include "raylib.h"

using namespace Utilities;

namespace Render
{
    class Window {
        public:
            Window() {};
            ~Window() { std::cout << "Window destroyed" << std::endl; };
            void startWindow(Vector2D);
            void closeWindow();
            bool isWindowClosing();
            bool isWindowOpen();
            void setWindowSize(Vector2D);
            void startRender();
            void clearBackground(Color);
            void endRender();
            void setFPS(int fps);
        protected:
        private:
            bool _windowOpen;
    };
}
