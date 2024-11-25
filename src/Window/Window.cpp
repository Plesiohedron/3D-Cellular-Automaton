#include "Window.h"

#include "../Events/Events.h"
#include <iostream>

Window::Window(int window_width, int window_height, const char* window_title) 
    : width(window_width), height(window_height) {
    if (!glfwInit()) {
        std::cout << "Failed to initialize GLFW!" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, window_title, nullptr, nullptr);
    if (!window) {
        std::cout << "Failed to create window!" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        std::cout << "Failed to initialize GLEW!" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    glViewport(0, 0, width, height);

    Events::Initialize(this);
}

bool Window::IsShouldClose() const {
    return glfwWindowShouldClose(window);
}

void Window::SetShouldClose(bool flag) const {
    glfwSetWindowShouldClose(window, flag);
}

void Window::SwapBuffers() const {
    glfwSwapBuffers(window);
}

float Window::GetAspect() const {
    return static_cast<float>(width) / height;
}

Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
}
