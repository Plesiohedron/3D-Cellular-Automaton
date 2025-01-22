#include "VAO.h"

GL::VBO::VBO() {
    glGenBuffers(1, &VBO_);
}

GL::VBO& GL::VBO::operator=(VBO&& other) {
    if (this != &other) {
        glDeleteBuffers(1, &VBO_);
        VBO_ = other.VBO_;
        other.VBO_ = 0;
    }

    return *this;
}

GL::VBO::~VBO() {
    glDeleteBuffers(1, &VBO_);
}

void GL::VBO::Bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
}

void GL::VBO::Unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GL::VBO::BindAsReadBuffer() const {
    glBindBuffer(GL_COPY_READ_BUFFER, VBO_);
}

void GL::VBO::BindAsWriteBuffer() const {
    glBindBuffer(GL_COPY_WRITE_BUFFER, VBO_);
}

void GL::VBO::UnbindAsReadBuffer() const {
    glBindBuffer(GL_COPY_READ_BUFFER, 0);
}

void GL::VBO::UnbindAsWriteBuffer() const {
    glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
}

void GL::VBO::Allocate(size_t vertex_data_size, GLuint binding_point, GLint element_size, GLenum element_type, GLenum draw_type, GLuint divisor) {
    Bind();
    glBufferData(GL_ARRAY_BUFFER, vertex_data_size, nullptr, draw_type);
    glVertexAttribIPointer(binding_point, element_size, element_type, 0, nullptr);
    glEnableVertexAttribArray(binding_point);

    if (divisor != 0) {
        glVertexAttribDivisor(binding_point, divisor);
    }
}

void GL::VBO::Assign(const void *vertex_data, size_t vertex_data_size, size_t offset) {
    glBufferSubData(GL_ARRAY_BUFFER, offset, vertex_data_size, vertex_data);
}

GL::VAO::VAO() {
    glGenVertexArrays(1, &VAO_);
}

GL::VAO::~VAO() {
    glDeleteVertexArrays(1, &VAO_);
}

void GL::VAO::Bind() const {
    glBindVertexArray(VAO_);
}

void GL::VAO::Unbind() const {
    glBindVertexArray(0);
}

void GL::CopyBuffer(size_t offset_read_buffer, size_t offset_write_buffer, size_t data_size) {
    glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 
                        offset_read_buffer, offset_write_buffer, data_size);
}
