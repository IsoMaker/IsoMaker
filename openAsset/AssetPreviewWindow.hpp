#pragma once
#include <thread>
#include <atomic>

class AssetPreviewWindow {
public:
    AssetPreviewWindow(int width = 400, int height = 300);
    ~AssetPreviewWindow();

    void open(); // Starts the window loop in a separate thread
    bool isOpen() const;

private:
    void run(); // Threaded window loop
    std::thread _thread;
    std::atomic<bool> _running;
    int _width, _height;
};
