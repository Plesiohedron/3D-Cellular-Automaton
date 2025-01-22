#include "Camera.h"

const float Camera::near = 0.01f;
const float Camera::far = 512.0f;

Camera::Camera(const glm::vec3& pos, float FOV) : position(pos), FOV(FOV) {
    UpdateVectors();
}

void Camera::UpdateVectors() {
    vector_up = glm::vec3(rotation * glm::vec4(0, 1, 0, 1));
    vector_front = glm::vec3(rotation * glm::vec4(0, 0, -1, 1));
    vector_right = glm::vec3(rotation * glm::vec4(1, 0, 0, 1));
}

void Camera::Rotate(float x, float y, float z) {
    rotation = glm::rotate(rotation, z, glm::vec3(0, 0, 1));
    rotation = glm::rotate(rotation, y, glm::vec3(0, 1, 0));
    rotation = glm::rotate(rotation, x, glm::vec3(1, 0, 0));

    UpdateVectors();
}

glm::mat4 Camera::GetProjection() const {
    return glm::perspective(FOV, Events::window->GetAspect(), near, far);
}

glm::mat4 Camera::GetView() const {
    return glm::lookAt(position, position + vector_front, vector_up);
}