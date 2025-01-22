#include "Program.h"

#include <stdio.h>

#ifdef _WIN32
#define PATH_MAX _MAX_PATH
#endif

#ifdef __unix__
#include <limits.h>
#endif 

GL::Program::Program(const char* filename) {
    char full_path[PATH_MAX];

    program_ = glCreateProgram();

    snprintf(full_path, sizeof(full_path), "%s%s%s", "./res/glsl/", filename, ".vert");
    vertex_shader_ = LoadShader(full_path, GL_VERTEX_SHADER);

    snprintf(full_path, sizeof(full_path), "%s%s%s", "./res/glsl/", filename, ".frag");
    fragment_shader_ = LoadShader(full_path, GL_FRAGMENT_SHADER);
}

GL::Program::~Program() {
    glDetachShader(program_, vertex_shader_);
    glDetachShader(program_, fragment_shader_);

    glDeleteShader(vertex_shader_);
    glDeleteShader(fragment_shader_);

    glDeleteProgram(program_);
}

void GL::Program::Link() const {
    glAttachShader(program_, vertex_shader_);
    glAttachShader(program_, fragment_shader_);
    glLinkProgram(program_);

    GLint status;
    glGetProgramiv(program_, GL_LINK_STATUS, &status);
    if (!status) {
        GLint log_length;
        glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &log_length);

        char* buf = new char[log_length];
        glGetProgramInfoLog(program_, log_length, nullptr, buf);

        fprintf(stderr, "%s\n", buf);
        fprintf(stderr, "Failed to link shader.\n");
        delete[] buf;
    }
}

void GL::Program::Use() const {
    glUseProgram(program_);
}

void GL::Program::Unuse() const {
    glUseProgram(0);
}

void GL::Program::BindAttribute(GLuint index, const char* name) const {
    glBindAttribLocation(program_, index, name);
}

GLint GL::Program::GetUniformLocation(const char* name) const {
    return glGetUniformLocation(program_, name);
}

void GL::Program::UniformInt(GLint location, int value) const {
    glUniform1i(location, value);
}

void GL::Program::UniformMatrix(GLint location, const glm::mat4& matrix) const {
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void GL::Program::UniformTexture(GLint location, GLuint number) const {
    glUniform1ui(location, number);
}

GLuint GL::Program::LoadShader(const char* path, GLenum shader_type) const {
    GLuint shader = glCreateShader(shader_type);

    const char* code = ReadCode(path);
    glShaderSource(shader, 1, &code, nullptr);
    glCompileShader(shader);
    delete[] code;

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        GLint log_length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);

        char* buf = new char[log_length];
        glGetShaderInfoLog(program_, log_length, nullptr, buf);

        fprintf(stderr, "%s: %s\n", path, buf);
        fprintf(stderr, "Failed to compile shader.\n");
        delete[] buf;
    }

    return shader;
}

char* GL::Program::ReadCode(const char *path) const {
    FILE* file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "%s\n", path);
        fprintf(stderr, "Failed to open file.");
        return nullptr;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char* buf = new char[file_size + 1];

    if (fread(buf, sizeof(char), file_size, file) != file_size) {
        fprintf(stderr, "%s\n", path);
        fprintf(stderr, "Failed to read file.");
        fclose(file);
        delete[] buf;
        return nullptr;
    }

    buf[file_size] = '\0';
    fclose(file);
    return buf;
}
