#pragma once

#include "../Window/Window.h"

namespace Events {
    const int KEYBOARD_BUTTONS_COUNT = 1024;
    const int MOUSE_BUTTONS_COUNT = 8;
    extern bool keys[KEYBOARD_BUTTONS_COUNT + MOUSE_BUTTONS_COUNT];

    extern unsigned int current_frame;
    extern unsigned int frames[KEYBOARD_BUTTONS_COUNT + MOUSE_BUTTONS_COUNT];

    extern float cursor_x;
    extern float cursor_y;
    extern float cursor_delta_x;
    extern float cursor_delta_y;
    extern bool cursor_is_moving;
    extern bool cursor_is_locked;

    extern Window* window;


    void Initialize(Window* window);
    void PollEvents();

    bool KeyIsClicked(int key);
    bool KeyIsPressed(int key);
    bool MouseIsClicked(int button);
    bool MouseIsPressed(int button);
    void SwitchCursor();

    void WindowResizeCallback(GLFWwindow* window, int width, int height);
    void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    void WindowIconifyCallback(GLFWwindow* window, int iconified);
};  // namespace Events
