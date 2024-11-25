#include "Chunks.h"

#include <iostream>

Chunks::Chunks(const glm::ivec3& sizes) {
    Chunks::sizes = sizes;
    Chunks::count = sizes.x * sizes.y * sizes.z;

    chunks_ = new Chunk*[Chunks::count];
    offset_models_ = new Model[Chunks::count];
    commands_ = new IndirectCommand[Chunks::count];
    // Initializing and meshing all chunks
    size_t total_VBO_size = 0;
    for (int i = 0, y = 0; y < sizes.y; ++y) {
        for (int z = 0; z < sizes.z; ++z) {
            for (int x = 0; x < sizes.x; ++x, ++i) {
                chunks_[i] = new Chunk({x, y, z}, Chunks::sizes);
                offset_models_[i] = {chunks_[i]->global_coordinates.x * Chunk::WIDTH,
                                     chunks_[i]->global_coordinates.y * Chunk::HEIGHT,
                                     chunks_[i]->global_coordinates.z * Chunk::DEPTH, 0};

                chunks_[i]->Mesh();
                total_VBO_size += chunks_[i]->vertex_data_capacity;
            }
        }
    }
    // Allocation and assignation of buffers on the GPU
    unified_VAO_.Bind();
    unified_VAO_.AllocateVBO(total_VBO_size);

    std::cout << total_VBO_size << '\n';

    size_t offset = 0;
    for (int i = 0, y = 0; y < sizes.y; ++y) {
        for (int z = 0; z < sizes.z; ++z) {
            for (int x = 0; x < sizes.x; ++x, ++i) {
                unified_VAO_.FillVBOSection(chunks_[i]->vertex_data, chunks_[i]->vertex_data_size, offset);
                commands_[i] = IndirectCommand{4, static_cast<GLuint>(chunks_[i]->vertex_data_size),
                                               0, static_cast<GLuint>(offset)};
                offset += chunks_[i]->vertex_data_capacity;
            }
        }
    }

    offset_vectors_SSBO_.Assign(offset_models_, Chunks::count, 0);
    indirect_commands_.Assign(commands_, Chunks::count);
    // Creation, binding and linking the shader
    shader_ = new GL::Program("Chunks");
    shader_->BindAttribute(0, "position");
    shader_->Link();
    shader_->Use();

    uniform_projview_loc_ = shader_->GetUniformLocation("projview");
    uniform_texture_loc_ = shader_->GetUniformLocation("texture0");
    // Creation and binding the texture
    texture_ = new GL::Texture2D();
    texture_->SetImage(Image::LoadImage("Voxel.png"));
    texture_->Bind();
    shader_->UniformTexture(uniform_texture_loc_, 0);
}

void Chunks::Draw(const Camera& camera) const {
    // Send projview matrix to the GPU
    shader_->UniformMatrix(uniform_projview_loc_, camera.GetProjection() * camera.GetView());
    // Draw everything with only one draw call!
    glMultiDrawArraysIndirect(GL_TRIANGLE_STRIP, nullptr, Chunks::count, 0);
}

Chunks::~Chunks() {
    for (int i = 0; i < Chunks::count; ++i) {
        delete chunks_[i];
    }
    delete[] chunks_;

    delete[] offset_models_;

    delete shader_;
    delete texture_;
}
