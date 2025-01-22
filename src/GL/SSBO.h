#pragma once
#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/gl.h>

namespace GL {
    class SSBO {
    private:
        GLuint SSBO_;

    public:
        SSBO();
        ~SSBO();

        void Bind() const;
        void Unbind() const;

        void Assign(const void* data, size_t data_size, GLuint binding_point, GLenum draw_type);
    };
}  // namespace GL