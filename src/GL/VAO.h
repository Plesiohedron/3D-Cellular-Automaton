#pragma once
#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/gl.h>

#include <inttypes.h>

using Vertex = uint16_t;

namespace GL {
    class VAO {
    private:
        GLuint VAO_;
        GLuint VBO_;

    public:
        VAO();
        VAO(const VAO&) = delete;
        ~VAO();

        void Bind() const;
        void Unbind() const;

        void AllocateVBO(size_t vertex_data_size);
        void FillVBOSection(const Vertex* vertex_data, size_t vertex_data_size, size_t offset);
    };
}  // namespace GL
