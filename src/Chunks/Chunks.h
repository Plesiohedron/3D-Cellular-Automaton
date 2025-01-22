#pragma once

#include "../GL/Program.h"
#include "../GL/Texture2D.h"
#include "../GL/VAO.h"
#include "../GL/SSBO.h"
#include "../GL/IBO.h"
#include "../GL/UBO.h"
#include "../Camera/Camera.h"

#include "Chunk.h"

class Chunks {
    friend class Engine;

private:
    static const glm::vec4 vertex_offsets_[Chunk::FACE_COUNT_PER_CUBE][Chunk::VERTEX_COUNT_PER_FACE];
    glm::vec4* chunk_offsets_;
    IndirectCommand* commands_;

    GL::VAO unified_VAO_;
    GL::VBO unified_VBO_;
    GL::SSBO chunk_offsets_SSBO_;
    GL::IBO indirect_commands_;
    GL::UBO vertex_offsets_UBO_;

    GL::Texture2D* texture_;
    GL::Program* shader_;

    GLint uniform_texture_loc_;
    GLint uniform_projview_loc_;

    Chunk** chunks_;

    // For frustum culling
    glm::vec3 frustum_TL;
    glm::vec3 frustum_TR;
    glm::vec3 frustum_BR;
    glm::vec3 frustum_BL;

    glm::mat4 rotation = glm::mat4(1.0f);

    const float frustum_length = 32.0f;
    const float h_near;
    const float w_near;

public:
    glm::ivec3 center = {0, 0, 0};
    glm::ivec3 sizes;
    int count;
    bool debug_mode = false;

    size_t total_VBO_size = 0;
    mutable size_t actual_total_VBO_size = 0;
    mutable bool is_data_modified = false;

private:
    Chunks(const glm::ivec3& sizes, float FOV);
    Chunks(const Chunks&) = delete;
    ~Chunks();

public:
    void Draw(const Camera& camera) const;

    Chunk* GetChunk(int x, int y, int z) const;

    void UpdateVertexData();

    void BaseSideCulling(const glm::vec3& global_chunk_position);
    void SideCulling(const glm::vec3& previous_chunk_position, const glm::vec3& current_chunk_position);

    void FrustumRotate(const Camera& camera);
    void FrustumCulling(const glm::vec3& global_chunk_position);
    void FrustumRayCast(const glm::vec3& position, const glm::vec3& direction, float ray_length) const;
};
