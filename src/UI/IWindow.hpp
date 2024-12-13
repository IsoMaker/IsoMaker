#pragma once
#include <raylib.h>

class IWindow {
    public:
        virtual ~IWindow() = default;

        virtual void Draw() const = 0;

        virtual void Resize(int x, int y, int width, int height) = 0;

        virtual void Update() = 0;

        virtual void HandleInput() = 0;

        virtual int GetX() const = 0;
        virtual int GetY() const = 0;
        virtual int GetWidth() const = 0;
        virtual int GetHeight() const = 0;
};