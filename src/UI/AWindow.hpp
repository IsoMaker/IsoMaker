#pragma once
#include "IWindow.hpp"

class AWindow : public IWindow {

    public:
        AWindow(int x, int y, int width, int height);
        virtual ~AWindow() = default;

        void Resize(int x, int y, int width, int height) override;

        int GetX() const override { return x; }
        int GetY() const override { return y; }
        int GetWidth() const override { return width; }
        int GetHeight() const override { return height; }

    protected:
        int x, y, width, height;
};