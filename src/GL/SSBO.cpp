#include "SSBO.h"

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

void GL::SSBO::Assign(const void* data, size_t data_size, GLuint binding_point, GLenum draw_type) {
    Bind();
    glBufferData(GL_SHADER_STORAGE_BUFFER, data_size, data, draw_type);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding_point, SSBO_);
}