#include "SSBO.h"

#include <glm/gtc/type_ptr.hpp>

GL::SSBO::SSBO() {
    glGenBuffers(1, &SSBO_);
}

GL::SSBO::~SSBO() {
    glDeleteBuffers(1, &SSBO_);
}

void GL::SSBO::Bind() const {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO_);
}

void GL::SSBO::Unbind() const {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void GL::SSBO::Assign(const Model* offsets_data, size_t data_size, GLuint binding_point) {
    Bind();
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Model) * data_size, offsets_data, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point, SSBO_);
}