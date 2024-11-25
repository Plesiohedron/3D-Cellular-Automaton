#pragma once

#include "../GL/Program.h"
#include "../GL/Texture2D.h"
#include "../GL/VAO.h"
#include "../GL/SSBO.h"
#include "../GL/IBO.h"
#include "../Camera/Camera.h"

#include "Chunk.h"

using Model = glm::vec4;

class Chunks {
    friend class Engine;

private:
    Chunk** chunks_;

    Model* offset_models_;
    IndirectCommand* commands_;

    GL::VAO unified_VAO_;
    GL::SSBO offset_vectors_SSBO_;
    GL::IBO indirect_commands_;

    GL::Texture2D* texture_;
    GL::Program* shader_;

    GLint uniform_texture_loc_;
    GLint uniform_projview_loc_;

public:
    glm::ivec3 sizes;
    int count;
    bool debug_mode = false;

private:
    Chunks(const glm::ivec3& sizes);
    Chunks(const Chunks&) = delete;
    ~Chunks();

public:
    void Draw(const Camera& camera) const;
};
