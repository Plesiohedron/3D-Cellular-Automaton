#include "Chunk.h"
#include "Chunks.h"

#include <glm/gtc/noise.hpp>

Chunk::Chunk(const glm::ivec3& coordinates, const glm::ivec3& chunks_sizes, const Chunks* chunks) {
    global_coordinates = {coordinates.x - chunks_sizes.x / 2, coordinates.y, coordinates.z - chunks_sizes.z / 2};
    local_coordinates = {coordinates.x, coordinates.y, coordinates.z};
    chunks_ = chunks;
    
    voxels_ = new Voxel[Chunk::VOLUME];

    for (int i = 0; i < FACE_COUNT_PER_CUBE; ++i) {
        vertex_data_size[i] = 0;
        vertex_data_capacity[i] = BASE_VERTEX_DATA_CAPACITY;
        vertex_data[i] = new Vertex[vertex_data_capacity[i]];
    }

    // Assignation of voxels
    for (int i = 0, y = 0; y < Chunk::HEIGHT; ++y) {
        for (int z = 0; z < Chunk::DEPTH; ++z) {
            for (int x = 0; x < Chunk::WIDTH; ++x, ++i) {
                int global_x = x + global_coordinates.x * Chunk::WIDTH;
                int global_y = y + global_coordinates.y * Chunk::DEPTH;
                int global_z = z + global_coordinates.z * Chunk::HEIGHT;

                // You may play with chunk generation
                //Voxel id = global_y <= std::sin(0.1 * global_x) * 10 + std::cos(0.1 * global_z) * 10;
                Voxel id = glm::perlin(glm::vec3(global_x * 0.0125f, global_y * 0.0125f, global_z * 0.0125f)) > 0.1f;

                /*
                Voxel id = 0;
                if (global_z == -80) {
                    id = 1;
                }
                */

                voxels_[i] = id;
            }
        }
    }
}

bool Chunk::IsBlocked(int x, int y, int z) const {
    if (0 <= x && x < Chunk::WIDTH) {
        if (0 <= y && y < Chunk::HEIGHT) {
            if (0 <= z && z < Chunk::DEPTH) {
                return voxels_[(y * Chunk::DEPTH + z) * Chunk::WIDTH + x];
            } else if (z == -1) {
                if (Chunk* chunk = chunks_->GetChunk(local_coordinates.x, local_coordinates.y, local_coordinates.z - 1)) {
                    return chunk->voxels_[(y * Chunk::DEPTH + (Chunk::DEPTH - 1)) * Chunk::WIDTH + x];
                }
            } else {
                if (Chunk* chunk = chunks_->GetChunk(local_coordinates.x, local_coordinates.y, local_coordinates.z + 1)) {
                    return chunk->voxels_[(y * Chunk::DEPTH + 0) * Chunk::WIDTH + x];
                }
            }
        } else if (y == -1) {
            if (Chunk* chunk = chunks_->GetChunk(local_coordinates.x, local_coordinates.y - 1, local_coordinates.z)) {
                return chunk->voxels_[((Chunk::HEIGHT - 1) * Chunk::DEPTH + z) * Chunk::WIDTH + x];
            }
        } else {
            if (Chunk* chunk = chunks_->GetChunk(local_coordinates.x, local_coordinates.y + 1, local_coordinates.z)) {
                return chunk->voxels_[(0 * Chunk::DEPTH + z) * Chunk::WIDTH + x];
            }
        }
    } else if (x == -1) {
        if (Chunk* chunk = chunks_->GetChunk(local_coordinates.x - 1, local_coordinates.y, local_coordinates.z)) {
            return chunk->voxels_[(y * Chunk::DEPTH + z) * Chunk::WIDTH + (Chunk::WIDTH - 1)];
        }
    } else {
        if (Chunk* chunk = chunks_->GetChunk(local_coordinates.x + 1, local_coordinates.y, local_coordinates.z)) {
            return chunk->voxels_[(y * Chunk::DEPTH + z) * Chunk::WIDTH + 0];
        }
    }

    return false;
}

void Chunk::PushBack(Vertex vertex, int index) {
    Vertex* data = vertex_data[index];
    size_t& data_size = vertex_data_size[index];
    size_t& data_capacity = vertex_data_capacity[index];

    data[data_size++] = vertex;

    if (data_size == data_capacity) {
        Vertex* new_data = new Vertex[data_capacity *= 2];
        memcpy(new_data, data, sizeof(Vertex) * data_size);
        delete[] data;
        vertex_data[index] = new_data;
    }
}

void Chunk::Mesh() {
    for (int i = 0, y = 0; y < Chunk::HEIGHT; ++y) {
        for (int z = 0; z < Chunk::DEPTH; ++z) {
            for (int x = 0; x < Chunk::WIDTH; ++x, ++i) {
                Voxel id = voxels_[i];
                if (!id) {
                    continue;
                }

                // Adding to the buffer only those faces that aren't blocked by other voxels
                // Packing faces' direction and coordinates
                Vertex vertex = (x << 8) | (y << 4) | z;
                if (!IsBlocked(x - 1, y, z)) {
                    PushBack(vertex, 0);
                }
                if (!IsBlocked(x + 1, y, z)) {
                    PushBack(vertex, 1);
                }
                if (!IsBlocked(x, y - 1, z)) {
                    PushBack(vertex, 2);
                }
                if (!IsBlocked(x, y + 1, z)) {
                    PushBack(vertex, 3);
                }
                if (!IsBlocked(x, y, z - 1)) {
                    PushBack(vertex, 4);
                }
                if (!IsBlocked(x, y, z + 1)) {
                    PushBack(vertex, 5);
                }
            }
        }
    }
}

Chunk::~Chunk() {
    delete[] voxels_;

    for (int i = 0; i < FACE_COUNT_PER_CUBE; ++i) {
        delete[] vertex_data[i];
    }
}
