#include "Chunks.h"

const glm::vec4 Chunks::vertex_offsets_[Chunk::FACE_COUNT_PER_CUBE][Chunk::VERTEX_COUNT_PER_FACE] = {
    {glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 0, 0), glm::vec4(0, 1, 1, 0), glm::vec4(0, 0, 1, 0)},
    {glm::vec4(1, 0, 0, 0), glm::vec4(1, 1, 0, 0), glm::vec4(1, 0, 1, 0), glm::vec4(1, 1, 1, 0)},
    {glm::vec4(1, 0, 0, 0), glm::vec4(1, 0, 1, 0), glm::vec4(0, 0, 0, 0), glm::vec4(0, 0, 1, 0)},
    {glm::vec4(0, 1, 0, 0), glm::vec4(0, 1, 1, 0), glm::vec4(1, 1, 0, 0), glm::vec4(1, 1, 1, 0)},
    {glm::vec4(1, 1, 0, 0), glm::vec4(1, 0, 0, 0), glm::vec4(0, 1, 0, 0), glm::vec4(0, 0, 0, 0)},
    {glm::vec4(0, 1, 1, 0), glm::vec4(0, 0, 1, 0), glm::vec4(1, 1, 1, 0), glm::vec4(1, 0, 1, 0)},
};

Chunks::Chunks(const glm::ivec3& sizes, float FOV) : h_near(tan(FOV / 2)), w_near(h_near * Events::window->GetAspect()) {
    Chunks::sizes = sizes;
    Chunks::count = sizes.x * sizes.y * sizes.z;

    frustum_TL = glm::normalize(glm::vec3(-w_near, +h_near, 1));
    frustum_TR = glm::normalize(glm::vec3(+w_near, +h_near, 1));
    frustum_BR = glm::normalize(glm::vec3(+w_near, -h_near, 1));
    frustum_BL = glm::normalize(glm::vec3(-w_near, -h_near, 1));

    chunks_ = new Chunk*[Chunks::count]{nullptr};
    chunk_offsets_ = new glm::vec4[Chunks::count];
    commands_ = new IndirectCommand[Chunks::count * Chunk::FACE_COUNT_PER_CUBE];
    // Initializing all chunks
    for (int i = 0, y = 0; y < sizes.y; ++y) {
        for (int z = 0; z < sizes.z; ++z) {
            for (int x = 0; x < sizes.x; ++x, ++i) {
                Chunk* chunk = new Chunk({x, y, z}, Chunks::sizes, this);
                chunk_offsets_[i] = {chunk->global_coordinates.x * Chunk::WIDTH,
                                     chunk->global_coordinates.y * Chunk::HEIGHT,
                                     chunk->global_coordinates.z * Chunk::DEPTH, 0};

                chunks_[i] = chunk;
            }
        }
    }
    // Meshing chunks
    for (int i = 0, y = 0; y < sizes.y; ++y) {
        for (int z = 0; z < sizes.z; ++z) {
            for (int x = 0; x < sizes.x; ++x, ++i) {
                Chunk* chunk = chunks_[i];
                chunk->Mesh();

                for (int k = 0; k < Chunk::FACE_COUNT_PER_CUBE; ++k) {
                    total_VBO_size += chunk->vertex_data_capacity[k];
                }
            }
        }
    }

    // Allocation and assignation of buffers on the GPU
    //printf("%zu\n", total_VBO_size);
    actual_total_VBO_size = total_VBO_size;

    unified_VAO_.Bind();
    unified_VBO_.Bind();
    unified_VBO_.Allocate(sizeof(Vertex) * total_VBO_size, 0, 1, GL_UNSIGNED_SHORT, GL_STATIC_DRAW, 1);

    size_t offset = 0;
    for (int i = 0, j = 0; i < Chunks::count; ++i) {
        const Chunk* chunk = chunks_[i];
        for (int k = 0; k < Chunk::FACE_COUNT_PER_CUBE; ++k, ++j) {
            unified_VBO_.Assign(chunk->vertex_data[k], sizeof(Vertex) * chunk->vertex_data_size[k], sizeof(Vertex) * offset);
            commands_[j] = IndirectCommand{Chunk::VERTEX_COUNT_PER_FACE, static_cast<GLuint>(chunk->vertex_data_size[k]),
                                           0, static_cast<GLuint>(offset)};
            offset += chunk->vertex_data_capacity[k];
        }
    }

    chunk_offsets_SSBO_.Assign(chunk_offsets_, sizeof(glm::vec4) * Chunks::count, 0, GL_STATIC_DRAW);
    vertex_offsets_UBO_.Assign(vertex_offsets_, sizeof(glm::vec4) * Chunk::FACE_COUNT_PER_CUBE * Chunk::VERTEX_COUNT_PER_FACE, 0, GL_STATIC_DRAW);
    indirect_commands_.Assign(commands_, Chunks::count * Chunk::FACE_COUNT_PER_CUBE, GL_DYNAMIC_DRAW);
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
    // Draw everything with only one draw call
    glMultiDrawArraysIndirect(GL_TRIANGLE_STRIP, nullptr, Chunks::count * Chunk::FACE_COUNT_PER_CUBE, 0);
}

Chunk* Chunks::GetChunk(int x, int y, int z) const {
    if (0 <= x && x < sizes.x && 0 <= y && y < sizes.y && 0 <= z && z < sizes.z) {
        return chunks_[(y * sizes.z + z) * sizes.x + x];
    }

    return nullptr;
}

void Chunks::UpdateVertexData() {
    if (total_VBO_size == actual_total_VBO_size) {
        for (int i = 0, j = 0; i < Chunks::count; ++i) {
            Chunk* chunk = chunks_[i];
            if (chunk->is_modified) {
                int index = j;
                for (int k = 0; k < Chunk::FACE_COUNT_PER_CUBE; ++k, ++j) {
                    unified_VBO_.Assign(chunk->vertex_data[k], sizeof(Vertex) * chunk->vertex_data_size[k], sizeof(Vertex) * commands_[j].baseInstance);
                    commands_[j].instanceCount = static_cast<GLuint>(chunk->vertex_data_size[k]);
                }
                chunk->is_modified = false;
                indirect_commands_.SubAssign(commands_ + index, 6, index);
            }
        }
    } else {
        GL::VBO new_unified_VBO;
        new_unified_VBO.Bind();
        new_unified_VBO.Allocate(sizeof(Vertex) * actual_total_VBO_size, 0, 1, GL_UNSIGNED_SHORT, GL_STATIC_DRAW, 1);

        unified_VBO_.BindAsReadBuffer();
        new_unified_VBO.BindAsWriteBuffer();

        size_t offset = 0;
        for (int i = 0, j = 0; i < Chunks::count; ++i) {
            Chunk* chunk = chunks_[i];
            if (chunk->is_modified) {
                for (int k = 0; k < Chunk::FACE_COUNT_PER_CUBE; ++k, ++j) {
                    new_unified_VBO.Assign(chunk->vertex_data[k], sizeof(Vertex) * chunk->vertex_data_size[k], sizeof(Vertex) * offset);
                    commands_[j].instanceCount = static_cast<GLuint>(chunk->vertex_data_size[k]);
                    commands_[j].baseInstance = static_cast<GLuint>(offset);
                    offset += chunk->vertex_data_capacity[k];
                }

                chunk->is_modified = false;
            } else {
                for (int k = 0; k < Chunk::FACE_COUNT_PER_CUBE; ++k, ++j) {
                    GL::CopyBuffer(sizeof(Vertex) * commands_[j].baseInstance, sizeof(Vertex) * offset, sizeof(Vertex) * commands_[j].instanceCount);
                    commands_[j].baseInstance = static_cast<GLuint>(offset);
                    offset += chunk->vertex_data_capacity[k];
                }
            }
        }

        unified_VBO_.UnbindAsReadBuffer();
        new_unified_VBO.UnbindAsWriteBuffer();

        indirect_commands_.SubAssign(commands_, Chunk::FACE_COUNT_PER_CUBE * Chunks::count, 0);
        unified_VBO_ = std::move(new_unified_VBO);
    }
}

void Chunks::FrustumRotate(const Camera& camera) {
    frustum_TL = glm::vec3(camera.rotation * glm::vec4(glm::normalize(glm::vec3(-w_near, +h_near, -1)), 1));
    frustum_TR = glm::vec3(camera.rotation * glm::vec4(glm::normalize(glm::vec3(+w_near, +h_near, -1)), 1));
    frustum_BR = glm::vec3(camera.rotation * glm::vec4(glm::normalize(glm::vec3(+w_near, -h_near, -1)), 1));
    frustum_BL = glm::vec3(camera.rotation * glm::vec4(glm::normalize(glm::vec3(-w_near, -h_near, -1)), 1));

    rotation = glm::mat4(1.0f);
    rotation = glm::rotate(rotation, -camera.camera_rotation_Y, glm::vec3(1, 0, 0));
    rotation = glm::rotate(rotation, -camera.camera_rotation_X, glm::vec3(0, 1, 0));
}

void Chunks::FrustumCulling(const glm::vec3& global_chunk_position) {
    for (int i = 0; i < Chunks::count; ++i) {
        chunks_[i]->is_visible = false;
    }

    FrustumRayCast(global_chunk_position, frustum_TL, frustum_length);
    FrustumRayCast(global_chunk_position, frustum_TR, frustum_length);
    FrustumRayCast(global_chunk_position, frustum_BR, frustum_length);
    FrustumRayCast(global_chunk_position, frustum_BL, frustum_length);

    for (int global_z = (center.z - sizes.z / 2);
             global_z <= (center.z + (sizes.z + 1) / 2); ++global_z) {
        for (int global_x = (center.x - sizes.x / 2);
                 global_x <= (center.x + (sizes.x + 1) / 2); ++global_x) {
            for (int global_y = 0; global_y <= sizes.y; ++global_y) {
                glm::vec3 vertex = glm::vec3(rotation * glm::vec4(glm::vec3(global_x, global_y, global_z) - global_chunk_position, 1));

                if (vertex.z >= -frustum_length &&
                    Events::window->GetAspect() / h_near * vertex.z <= -std::max(std::abs(vertex.x), std::abs(vertex.y))) {

                    int chunk_local_x = global_x - center.x + sizes.x / 2;
                    int chunk_local_y = global_y;
                    int chunk_local_z = global_z - center.z + sizes.z / 2;

                    if (chunk_local_x - 1 >= 0 && chunk_local_y - 1 >= 0 && chunk_local_z - 1 >= 0) {
                        chunks_[((chunk_local_y - 1) * sizes.z + (chunk_local_z - 1)) * sizes.x + (chunk_local_x - 1)]
                            ->is_visible = true;
                    }
                    if (chunk_local_x - 1 >= 0 && chunk_local_y - 1 >= 0 && chunk_local_z < sizes.z) {
                        chunks_[((chunk_local_y - 1) * sizes.z + chunk_local_z) * sizes.x + (chunk_local_x - 1)]
                            ->is_visible = true;
                    }
                    if (chunk_local_x - 1 >= 0 && chunk_local_y < sizes.y && chunk_local_z - 1 >= 0) {
                        chunks_[(chunk_local_y * sizes.z + (chunk_local_z - 1)) * sizes.x + (chunk_local_x - 1)]
                            ->is_visible = true;
                    }
                    if (chunk_local_x - 1 >= 0 && chunk_local_y < sizes.y && chunk_local_z < sizes.z) {
                        chunks_[(chunk_local_y * sizes.z + chunk_local_z) * sizes.x + (chunk_local_x - 1)]
                            ->is_visible = true;
                    }
                    if (chunk_local_x < sizes.x && chunk_local_y - 1 >= 0 && chunk_local_z - 1 >= 0) {
                        chunks_[((chunk_local_y - 1) * sizes.z + (chunk_local_z - 1)) * sizes.x + chunk_local_x]
                            ->is_visible = true;
                    }
                    if (chunk_local_x < sizes.x && chunk_local_y - 1 >= 0 && chunk_local_z < sizes.z) {
                        chunks_[((chunk_local_y - 1) * sizes.z + chunk_local_z) * sizes.x + chunk_local_x]
                            ->is_visible = true;
                    }
                    if (chunk_local_x < sizes.x && chunk_local_y < sizes.y && chunk_local_z - 1 >= 0) {
                        chunks_[(chunk_local_y * sizes.z + (chunk_local_z - 1)) * sizes.x + chunk_local_x]
                            ->is_visible = true;
                    }
                    if (chunk_local_x < sizes.x && chunk_local_y < sizes.y && chunk_local_z < sizes.z) {
                        chunks_[(chunk_local_y * sizes.z + chunk_local_z) * sizes.x + chunk_local_x]
                            ->is_visible = true;
                    }
                }
            }
        }
    }

    for (int i = 0, j = 0; i < Chunks::count; ++i, j += Chunk::FACE_COUNT_PER_CUBE) {
        Chunk* chunk = chunks_[i];

        if (!chunk->is_visible && chunk->previously_is_visible) {
            GLuint temp[Chunk::FACE_COUNT_PER_CUBE];
            for (int k = 0; k < Chunk::FACE_COUNT_PER_CUBE; ++k) {
                temp[k] = commands_[j + k].instanceCount;
                commands_[j + k].instanceCount = 0;
            }

            indirect_commands_.SubAssign(commands_ + j, Chunk::FACE_COUNT_PER_CUBE, j);

            for (int k = 0; k < Chunk::FACE_COUNT_PER_CUBE; ++k) {
                commands_[j + k].instanceCount = temp[k];
            }
        } else if (chunk->is_visible && !chunk->previously_is_visible) {
            indirect_commands_.SubAssign(commands_ + j, Chunk::FACE_COUNT_PER_CUBE, j);
        }

        chunk->previously_is_visible = chunk->is_visible;
    }
}

void Chunks::FrustumRayCast(const glm::vec3& position, const glm::vec3& direction, float ray_length) const {
    float px = position.x - center.x + (sizes.x / 2);
    float py = position.y;
    float pz = position.z - center.z + (sizes.z / 2);

    float dx = direction.x;
    float dy = direction.y;
    float dz = direction.z;

    float t = 0.0f;
    int ix = floor(px);
    int iy = floor(py);
    int iz = floor(pz);

    int stepx = (dx > 0.0f) ? 1 : -1;
    int stepy = (dy > 0.0f) ? 1 : -1;
    int stepz = (dz > 0.0f) ? 1 : -1;

    float infinity = std::numeric_limits<float>::infinity();

    float txDelta = (dx == 0.0f) ? infinity : abs(1.0f / dx);
    float tyDelta = (dy == 0.0f) ? infinity : abs(1.0f / dy);
    float tzDelta = (dz == 0.0f) ? infinity : abs(1.0f / dz);

    float xdist = (stepx > 0) ? (ix + 1 - px) : (px - ix);
    float ydist = (stepy > 0) ? (iy + 1 - py) : (py - iy);
    float zdist = (stepz > 0) ? (iz + 1 - pz) : (pz - iz);

    float txMax = (txDelta < infinity) ? txDelta * xdist : infinity;
    float tyMax = (tyDelta < infinity) ? tyDelta * ydist : infinity;
    float tzMax = (tzDelta < infinity) ? tzDelta * zdist : infinity;

    while (t <= ray_length) {
        if (ix >= 0 && iy >= 0 && iz >= 0 && ix < sizes.x && iy < sizes.y && iz < sizes.z) {
            chunks_[(iy * sizes.z + iz) * sizes.x + ix]->is_visible = true;
        }

        if (txMax < tyMax) {
            if (txMax < tzMax) {
                ix += stepx;
                t = txMax;
                txMax += txDelta;
            } else {
                iz += stepz;
                t = tzMax;
                tzMax += tzDelta;
            }
        } else {
            if (tyMax < tzMax) {
                iy += stepy;
                t = tyMax;
                tyMax += tyDelta;
            } else {
                iz += stepz;
                t = tzMax;
                tzMax += tzDelta;
            }
        }
    }
}

void Chunks::BaseSideCulling(const glm::vec3& global_chunk_position) {
    for (int i = 0, j = 0; i < Chunks::count; ++i) {
        Chunk* chunk = chunks_[i];

        if (chunk->global_coordinates.x < global_chunk_position.x) {
            commands_[j++].instanceCount = 0;
            commands_[j++].instanceCount = static_cast<GLuint>(chunk->vertex_data_size[1]);
        } else if (chunk->global_coordinates.x > global_chunk_position.x) {
            commands_[j++].instanceCount = static_cast<GLuint>(chunk->vertex_data_size[0]);
            commands_[j++].instanceCount = 0;
        } else {
            commands_[j++].instanceCount = static_cast<GLuint>(chunk->vertex_data_size[0]);
            commands_[j++].instanceCount = static_cast<GLuint>(chunk->vertex_data_size[1]);
        }

        if (chunk->global_coordinates.y < global_chunk_position.y) {
            commands_[j++].instanceCount = 0;
            commands_[j++].instanceCount = static_cast<GLuint>(chunk->vertex_data_size[3]);
        } else if (chunk->global_coordinates.y > global_chunk_position.y) {
            commands_[j++].instanceCount = static_cast<GLuint>(chunk->vertex_data_size[2]);
            commands_[j++].instanceCount = 0;
        } else {
            commands_[j++].instanceCount = static_cast<GLuint>(chunk->vertex_data_size[2]);
            commands_[j++].instanceCount = static_cast<GLuint>(chunk->vertex_data_size[3]);
        }

        if (chunk->global_coordinates.z < global_chunk_position.z) {
            commands_[j++].instanceCount = 0;
            commands_[j++].instanceCount = static_cast<GLuint>(chunk->vertex_data_size[5]);
        } else if (chunk->global_coordinates.z > global_chunk_position.z) {
            commands_[j++].instanceCount = static_cast<GLuint>(chunk->vertex_data_size[4]);
            commands_[j++].instanceCount = 0;
        } else {
            commands_[j++].instanceCount = static_cast<GLuint>(chunk->vertex_data_size[4]);
            commands_[j++].instanceCount = static_cast<GLuint>(chunk->vertex_data_size[5]);
        }
    }

    indirect_commands_.SubAssign(commands_, Chunk::FACE_COUNT_PER_CUBE * Chunks::count, 0);
}

void Chunks::SideCulling(const glm::vec3& previous_chunk_position, const glm::vec3& current_chunk_position) {
    glm::ivec3 previous = previous_chunk_position;
    glm::ivec3 current = current_chunk_position;

    glm::ivec3 local_chunk = current_chunk_position;
    local_chunk.x += -0 + sizes.x / 2;
    local_chunk.z += -0 + sizes.z / 2;

    if (previous.x != current.x) {
        for (int delta_x = std::min(0, previous.x - current.x); delta_x <= std::max(0, previous.x - current.x); ++delta_x) {
            int x = local_chunk.x + delta_x;

            if (0 <= x && x < sizes.x) {
                for (int y = 0; y < sizes.y; ++y) {
                    for (int z = 0; z < sizes.z; ++z) {
                        int i = (y * sizes.z + z) * sizes.x + x;
                        int j = Chunk::FACE_COUNT_PER_CUBE * i;

                        if (delta_x > 0) {
                            commands_[j].instanceCount = static_cast<GLuint>(chunks_[i]->vertex_data_size[0]);
                            commands_[j + 1].instanceCount = 0;
                        } else if (delta_x < 0) {
                            commands_[j].instanceCount = 0;
                            commands_[j + 1].instanceCount = static_cast<GLuint>(chunks_[i]->vertex_data_size[1]);
                        } else if (delta_x == 0) {
                            commands_[j].instanceCount = static_cast<GLuint>(chunks_[i]->vertex_data_size[0]);
                            commands_[j + 1].instanceCount = static_cast<GLuint>(chunks_[i]->vertex_data_size[1]);
                        }

                        if (chunks_[i]->previously_is_visible) {
                            indirect_commands_.SubAssign(commands_ + j, 2, j);
                        }
                    }
                }
            }
        }
    }

    if (previous.y != current.y) {
        for (int delta_y = std::min(0, previous.y - current.y); delta_y <= std::max(0, previous.y - current.y); ++delta_y) {
            int y = local_chunk.y + delta_y;

            if (0 <= y && y < sizes.y) {
                for (int x = 0; x < sizes.x; ++x) {
                    for (int z = 0; z < sizes.z; ++z) {
                        int i = (y * sizes.z + z) * sizes.x + x;
                        int j = Chunk::FACE_COUNT_PER_CUBE * i + 2;

                        if (delta_y > 0) {
                            commands_[j].instanceCount = static_cast<GLuint>(chunks_[i]->vertex_data_size[2]);
                            commands_[j + 1].instanceCount = 0;
                        } else if (delta_y < 0) {
                            commands_[j].instanceCount = 0;
                            commands_[j + 1].instanceCount = static_cast<GLuint>(chunks_[i]->vertex_data_size[3]);
                        } else if (delta_y == 0) {
                            commands_[j].instanceCount = static_cast<GLuint>(chunks_[i]->vertex_data_size[2]);
                            commands_[j + 1].instanceCount = static_cast<GLuint>(chunks_[i]->vertex_data_size[3]);
                        }

                        if (chunks_[i]->previously_is_visible) {
                            indirect_commands_.SubAssign(commands_ + j, 2, j);
                        }
                    }
                }
            }
        }
    }

    if (previous.z != current.z) {
        for (int delta_z = std::min(0, previous.z - current.z); delta_z <= std::max(0, previous.z - current.z); ++delta_z) {
            int z = local_chunk.z + delta_z;

            if (0 <= z && z < sizes.z) {
                for (int x = 0; x < sizes.x; ++x) {
                    for (int y = 0; y < sizes.y; ++y) {
                        int i = (y * sizes.z + z) * sizes.x + x;
                        int j = Chunk::FACE_COUNT_PER_CUBE * i + 4;

                        if (delta_z > 0) {
                            commands_[j].instanceCount = static_cast<GLuint>(chunks_[i]->vertex_data_size[4]);
                            commands_[j + 1].instanceCount = 0;
                        } else if (delta_z < 0) {
                            commands_[j].instanceCount = 0;
                            commands_[j + 1].instanceCount = static_cast<GLuint>(chunks_[i]->vertex_data_size[5]);
                        } else if (delta_z == 0) {
                            commands_[j].instanceCount = static_cast<GLuint>(chunks_[i]->vertex_data_size[4]);
                            commands_[j + 1].instanceCount = static_cast<GLuint>(chunks_[i]->vertex_data_size[5]);
                        }

                        if (chunks_[i]->previously_is_visible) {
                            indirect_commands_.SubAssign(commands_ + j, 2, j);
                        }
                    }
                }
            }
        }
    }
}

Chunks::~Chunks() {
    for (int i = 0; i < Chunks::count; ++i) {
        delete chunks_[i];
    }
    delete[] chunks_;

    delete[] chunk_offsets_;

    delete shader_;
    delete texture_;
}
