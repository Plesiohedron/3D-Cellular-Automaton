#include "UBO.h"

GL::UBO::UBO() {
    glGenBuffers(1, &UBO_);
}

GL::UBO::~UBO() {
    glDeleteBuffers(1, &UBO_);
}

void GL::UBO::Bind() const {
    glBindBuffer(GL_UNIFORM_BUFFER, UBO_);
}

void GL::UBO::Unbind() const {
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void GL::UBO::Assign(const void* data, size_t data_size, GLuint binding_point, GLenum draw_type) {
    Bind();
    glBufferData(GL_UNIFORM_BUFFER, data_size, data, draw_type);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, UBO_);
}