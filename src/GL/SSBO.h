#pragma once
#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/gl.h>
#include <glm/glm.hpp>

using Model = glm::vec4;

namespace GL {
    class SSBO {
    private:
        GLuint SSBO_;

    public:
        SSBO();
        ~SSBO();

        void Bind() const;
        void Unbind() const;

        void Assign(const Model* offsets_data, size_t data_size, GLuint binding_point);
    };
}  // namespace GL