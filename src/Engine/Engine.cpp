#include "Engine.h"

#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

Engine::Engine(int window_width, int window_height, const char* window_title)
    : window_{window_width, window_height, window_title}, camera_{{0.0f, 12.0f, 12.0f}, glm::radians(90.0f)} {
    // In the beginning the camera directed to -Z, so we rotate it
    camera_.Rotate(0.0f, glm::radians(180.0f), 0.0f);
    camera_.camera_rotation_X = glm::radians(180.0f);
    // Creation of chunks
    chunks_ = new Chunks({10, 10, 10});
    // Some necessary magic
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClearColor(0.f, 0.f, 0.f, 1.0f);
    glfwSwapInterval(0);  // Off vertical synchronization
}

void Engine::MainLoop() {
    double last_time = glfwGetTime();
    double delta_time = 0.0;
    double current_time = 0.0;
    double total_time = 0.0;
    int frame_count = 0;
    // Speed of movement through the universe
    float speed = 15.0f;

    while (!window_.IsShouldClose()) {
        // For profiling
        current_time = glfwGetTime();
        delta_time = current_time - last_time;
        last_time = current_time;

        // std::cout << 1 / delta_time << '\n';

        if (!window_.is_iconfied) {
            // Event handling
            if (Events::KeyIsClicked(GLFW_KEY_ESCAPE)) {
                window_.SetShouldClose(true);
            }
            if (Events::KeyIsClicked(GLFW_KEY_TAB)) {
                Events::SwitchCursor();
            }

            if (Events::KeyIsPressed(GLFW_KEY_W)) {
                camera_.position += camera_.vector_front * delta_time * speed;
            }
            if (Events::KeyIsPressed(GLFW_KEY_S)) {
                camera_.position -= camera_.vector_front * delta_time * speed;
            }
            if (Events::KeyIsPressed(GLFW_KEY_D)) {
                camera_.position += camera_.vector_right * delta_time * speed;
            }
            if (Events::KeyIsPressed(GLFW_KEY_A)) {
                camera_.position -= camera_.vector_right * delta_time * speed;
            }
            if (Events::KeyIsPressed(GLFW_KEY_SPACE)) {
                camera_.position.y += delta_time * speed;
            }
            if (Events::KeyIsPressed(GLFW_KEY_LEFT_SHIFT)) {
                camera_.position.y -= delta_time * speed;
            }

            if (window_.is_resized) {
                window_.is_resized = false;
            }
            // Rotate camera if cursor is locked (on TAB)
            if (Events::cursor_is_locked) {
                camera_.camera_rotation_X += -2 * Events::cursor_delta_x / window_.height;
                camera_.camera_rotation_Y += -2 * Events::cursor_delta_y / window_.height;

                if (camera_.camera_rotation_Y < -glm::radians(90.0f)) {
                    camera_.camera_rotation_Y = -glm::radians(90.0f);
                } else if (camera_.camera_rotation_Y > glm::radians(90.0f)) {
                    camera_.camera_rotation_Y = glm::radians(90.0f);
                }

                camera_.rotation = glm::mat4(1.0f);
                camera_.Rotate(camera_.camera_rotation_Y, camera_.camera_rotation_X, 0.0f);
            }
            // For debug rendering
            if (Events::KeyIsClicked(GLFW_KEY_E)) {
                chunks_->debug_mode = !chunks_->debug_mode;
                if (chunks_->debug_mode) {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                } else {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }
            }

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            // Draw everything
            chunks_->Draw(camera_);
        }
        // Show the scene on the window
        window_.SwapBuffers();
        // Poll events from system
        Events::PollEvents();
        // For profiling
        total_time += delta_time;
        ++frame_count;
        if (total_time > 1.0) {
            std::cout << frame_count / total_time << '\n';
            frame_count = 0;
            total_time = 0;
        }
    }
}

Engine::~Engine() {
    delete chunks_;
}