#pragma once
#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/gl.h>

namespace GL {
    class VBO {
    private:
        GLuint VBO_;

    public:
        VBO();
        VBO(const VBO&) = delete;
        VBO& operator=(VBO&&);
        ~VBO();

        void Bind() const;
        void Unbind() const;

        void BindAsReadBuffer() const;
        void BindAsWriteBuffer() const;

        void UnbindAsReadBuffer() const;
        void UnbindAsWriteBuffer() const;

        void Allocate(size_t vertex_data_size, GLuint binding_point, GLint element_size, GLenum element_type, GLenum draw_type, GLuint divisor = 0);
        void Assign(const void* vertex_data, size_t vertex_data_size, size_t offset);
    };

    void CopyBuffer(size_t offset_read_buffer, size_t offset_write_buffer, size_t data_size);

    class VAO {
    private:
        GLuint VAO_;

    public:
        VAO();
        VAO(const VAO&) = delete;
        ~VAO();

        void Bind() const;
        void Unbind() const;
    };
}  // namespace GL
