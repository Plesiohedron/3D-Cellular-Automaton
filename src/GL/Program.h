#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/gl.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace GL {
    class Program {
    public:
        Program(const char* filename);
        ~Program();

        void Link() const;
        void Use() const;
        void Unuse() const;

        void BindAttribute(GLuint index, const char* name) const;
        GLint GetUniformLocation(const char* name) const;
        void UniformInt(GLint location, int value) const;
        void UniformMatrix(GLint uniform, const glm::mat4& matrix) const;
        void UniformTexture(GLint location, GLuint number) const;

    private:
        GLuint LoadShader(const char* path, const GLenum shader_type) const;
        char* ReadCode(const char* path) const;

    private:
        GLuint program_;
        GLuint vertex_shader_;
        GLuint fragment_shader_;
    };
}  // namespace GL
