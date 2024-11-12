#pragma once

#include "../Utilities/Vector.hpp"
#include "raylib.h"

using namespace Utilities;

namespace Render {

    class Window {
        public:
            Window();
            ~Window();
            void startwindow(Vector2D);
            void closeWindow();
            bool isWindowClosing();
            bool isWindowOpen();
            void setWindowSize(Vector2D);
            void startRender();
            void clearBackground();
            void endRender();
            void setFPS(int fps);
        private:
            bool _windowOpen;
    };

}
