#include "IBO.h"

GL::IBO::IBO() {
    glGenBuffers(1, &IBO_);
}

GL::IBO::~IBO() {
    glDeleteBuffers(1, &IBO_);
}

void GL::IBO::Bind() const {
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, IBO_);
}

void GL::IBO::Unbind() const {
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
}

void GL::IBO::Assign(const IndirectCommand *commands, size_t commands_size, GLenum draw_type) {
    Bind();
    glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(IndirectCommand) * commands_size, commands, draw_type);
}

void GL::IBO::SubAssign(const IndirectCommand *commands, size_t commands_size, size_t offset) {
    glBufferSubData(GL_DRAW_INDIRECT_BUFFER, sizeof(IndirectCommand) * offset, sizeof(IndirectCommand) * commands_size, commands);
}
