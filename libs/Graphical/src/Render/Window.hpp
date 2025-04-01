#pragma once

#include "../Utilities/Vector.hpp"
#include "raylib.h"

using namespace Utilities;

namespace Render {

    class Window {
        public:
            Window() {};
            ~Window() {};
            void startWindow(Vector2D, std::string);
            void closeWindow();
            bool isWindowClosing();
            bool isWindowOpen();
            void setWindowSize(Vector2D);
            void startRender();
            void clearBackground(Color);
            void endRender();
            void setFPS(int fps);
        private:
            bool _windowOpen;
    };

}
