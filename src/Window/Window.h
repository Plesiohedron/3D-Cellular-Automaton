#pragma once
#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window {
    friend class Engine;

public:
    bool IsShouldClose() const;
    void SetShouldClose(bool flag) const;
    void SwapBuffers() const;
    float GetAspect() const;

public:
    int width;
    int height;

    bool is_iconfied{false};
    bool is_resized{false};

    GLFWwindow* window;

private:
    Window(int window_width, int window_height, const char* window_title);
    ~Window();
};
