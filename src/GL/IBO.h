#pragma once
#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/gl.h>

struct IndirectCommand {
    GLuint count;
    GLuint instanceCount;
    GLuint first;
    GLuint baseInstance;
};

namespace GL {
    class IBO {
        private:
            GLuint IBO_;

        public:
            IBO();
            IBO(const IBO&) = delete;
            ~IBO();

            void Bind() const;
            void Unbind() const;

            void Assign(const IndirectCommand* commands, size_t commands_size, GLenum draw_type);
            void SubAssign(const IndirectCommand* commands, size_t commands_size, size_t offset);
    };
}  // namespace GL