#pragma once

#include "../Window/Window.h"
#include "../Camera/Camera.h"
#include "../Chunks/Chunks.h"

class Engine {
public:
    Engine(int window_width, int window_height, const char* window_title);
    ~Engine();

    // Function in which everything happens
    void MainLoop();

private:
    Window window_;
    Camera camera_;
    Chunks* chunks_;

private:
    Engine(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(const Engine&) = delete;
    Engine& operator=(Engine&&) = delete;

    // void SaveScreenshot(int width, int height, const char* filename) const;
};
