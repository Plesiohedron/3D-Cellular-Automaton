#pragma once
#define GLEW_STATIC

#include <GL/glew.h>
#include <GL/gl.h>

#include "../Load/Image.h"

namespace GL {
    class Texture2D {
    private:
        GLuint handle_;

    public:
        GLenum GL_format;

        int width;
        int height;

    public:
        Texture2D();
        Texture2D(const Texture2D&) = delete;
        ~Texture2D();

        void Bind() const;
        void Unbind() const;

        void SetImage(const Image& image);
    };
}  // namespace GL