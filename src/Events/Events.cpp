#include "Events.h"

namespace Events {
    bool keys[KEYBOARD_BUTTONS_COUNT + MOUSE_BUTTONS_COUNT]{false};
    // For clicking (not pressing)
    unsigned int current_frame = 0;
    unsigned int frames[KEYBOARD_BUTTONS_COUNT + MOUSE_BUTTONS_COUNT]{0};

    float cursor_x = 0.0f;
    float cursor_y = 0.0f;
    float cursor_delta_x = 0.0f;
    float cursor_delta_y = 0.0f;
    bool cursor_is_moving = false;
    bool cursor_is_locked = false;

    Window* window = nullptr;
}  // namespace Events

void Events::Initialize(Window* window) {
    Events::window = window;

    glfwSetWindowSizeCallback(Events::window->window, WindowResizeCallback);
    glfwSetKeyCallback(Events::window->window, KeyCallback);
    glfwSetMouseButtonCallback(Events::window->window, MouseButtonCallback);
    glfwSetCursorPosCallback(Events::window->window, CursorPosCallback);
    glfwSetWindowIconifyCallback(Events::window->window, WindowIconifyCallback);
}

void Events::PollEvents() {
    cursor_is_moving = false;
    cursor_delta_x = 0;
    cursor_delta_y = 0;
    ++current_frame;
    glfwPollEvents();
}

bool Events::KeyIsClicked(int key) {
    if (0 <= key && key < KEYBOARD_BUTTONS_COUNT) {
        return keys[key] && (frames[key] == current_frame);
    }

    return false;
}

bool Events::KeyIsPressed(int key) {
    if (0 <= key && key < KEYBOARD_BUTTONS_COUNT) {
        return keys[key];
    }

    return false;
}

bool Events::MouseIsClicked(int button) {
    return keys[KEYBOARD_BUTTONS_COUNT + button] && (frames[KEYBOARD_BUTTONS_COUNT + button] == current_frame);
}

bool Events::MouseIsPressed(int button) {
    return keys[KEYBOARD_BUTTONS_COUNT + button];
}

void Events::SwitchCursor() {
    cursor_is_locked = !cursor_is_locked;
    glfwSetInputMode(Events::window->window, GLFW_CURSOR, cursor_is_locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void Events::WindowResizeCallback(GLFWwindow* window, int width, int height) {
    if (width % 2 == 1) {
        --width;
    }
    if (height % 2 == 1) {
        --height;
    }

    glViewport(0, 0, width, height);

    Events::window->width = width;
    Events::window->height = height;

    Events::window->is_resized = true;
}

void Events::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        keys[key] = true;
        frames[key] = current_frame;
    } else if (action == GLFW_RELEASE) {
        keys[key] = false;
        frames[key] = current_frame;
    }
}

void Events::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        keys[KEYBOARD_BUTTONS_COUNT + button] = true;
        frames[KEYBOARD_BUTTONS_COUNT + button] = current_frame;
    } else if (action == GLFW_RELEASE) {
        keys[KEYBOARD_BUTTONS_COUNT + button] = false;
        frames[KEYBOARD_BUTTONS_COUNT + button] = current_frame;
    }
}

void Events::CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    float float_xpos = static_cast<float>(xpos);
    float float_ypos = static_cast<float>(ypos);

    cursor_delta_x += float_xpos - cursor_x;
    cursor_delta_y += float_ypos - cursor_y;

    cursor_is_moving = true;

    cursor_x = float_xpos;
    cursor_y = float_ypos;
}

void Events::WindowIconifyCallback(GLFWwindow* window, int iconified) {
    Events::window->is_iconified = iconified;
}
