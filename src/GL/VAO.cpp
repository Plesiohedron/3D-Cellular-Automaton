#include "VAO.h"

GL::VAO::VAO() {
    glGenVertexArrays(1, &VAO_);
}

void GL::VAO::Bind() const {
    glBindVertexArray(VAO_);
}

void GL::VAO::Unbind() const {
    glBindVertexArray(0);
}

void GL::VAO::AllocateVBO(size_t vertex_data_size) {
    glGenBuffers(1, &VBO_);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertex_data_size, nullptr, GL_DYNAMIC_DRAW);
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_SHORT, 0, nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribDivisor(0, 1);
}

void GL::VAO::FillVBOSection(const Vertex* vertex_data, size_t vertex_data_size, size_t offset) {
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertex) * offset, sizeof(Vertex) * vertex_data_size, vertex_data);
}

GL::VAO::~VAO() {
    glDeleteBuffers(1, &VBO_);
    glDeleteVertexArrays(1, &VAO_);
}