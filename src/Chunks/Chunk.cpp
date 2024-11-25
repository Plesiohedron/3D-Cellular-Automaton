#include "Chunk.h"

#include <glm/gtc/noise.hpp>

Chunk::Chunk(const glm::ivec3& coordinates, const glm::ivec3& chunks_sizes) {
    global_coordinates = {coordinates.x - chunks_sizes.x / 2, coordinates.y, coordinates.z - chunks_sizes.z / 2};
    local_coordinates = {coordinates.x, coordinates.y, coordinates.z};
    
    voxels_ = new Voxel[Chunk::VOLUME];
    vertex_data = static_cast<Vertex *>(malloc(vertex_data_capacity * sizeof(Vertex)));
    // Assignation of voxels
    for (int y = Chunk::HEIGHT - 1; y >= 0; --y) {
        for (int z = 0; z < Chunk::DEPTH; ++z) {
            for (int x = 0; x < Chunk::WIDTH; ++x) {
                int global_x = x + global_coordinates.x * Chunk::WIDTH;
                int global_y = y + global_coordinates.y * Chunk::DEPTH;
                int global_z = z + global_coordinates.z * Chunk::HEIGHT;

                // You may play with chunk generation
                //Voxel id = global_y <= std::sin(0.1 * global_x) * 10 + std::cos(0.1 * global_z) * 10;
                //Voxel id = glm::perlin(glm::vec3(global_x * 0.0125f, global_y * 0.0125f, global_z * 0.0125f)) > 0.1f;

                Voxel id = 0;
                if (global_coordinates == glm::ivec3{0, 0, 0} &&
                    (global_x + global_y + global_z) % 2 == 0) {
                    id = 1;
                }

                voxels_[(y * Chunk::DEPTH + z) * Chunk::WIDTH + x] = id;
            }
        }
    }
}

bool Chunk::IsBlocked(int x, int y, int z) const {
    if (0 <= x && x < Chunk::WIDTH && 0 <= y && y < Chunk::HEIGHT && 0 <= z && z < Chunk::DEPTH) {
        return voxels_[(y * Chunk::DEPTH + z) * Chunk::WIDTH + x];
    }

    return false;
}

void Chunk::PushBack(Vertex vertex) {
    vertex_data[vertex_data_size++] = vertex;

    if (vertex_data_size == vertex_data_capacity) {
        vertex_data = static_cast<Vertex *>(realloc(vertex_data, (vertex_data_capacity *= 2) * sizeof(Vertex)));
    }
}

void Chunk::Mesh() {
    for (int y = 0; y < Chunk::HEIGHT; ++y) {
        for (int z = 0; z < Chunk::DEPTH; ++z) {
            for (int x = 0; x < Chunk::WIDTH; ++x) {
                Voxel id = voxels_[(y * Chunk::DEPTH + z) * Chunk::WIDTH + x];
                if (!id) {
                    continue;
                }

                // Adding to the buffer only those faces that aren't blocked by other voxels
                // Packing faces' direction and coordinates
                Vertex vertex = (x << 8) | (y << 4) | z;
                if (!IsBlocked(x - 1, y, z)) {
                    PushBack((0 << 12) | vertex);
                }
                if (!IsBlocked(x + 1, y, z)) {
                    PushBack((1 << 12) | vertex);
                }
                if (!IsBlocked(x, y - 1, z)) {
                    PushBack((2 << 12) | vertex);
                }
                if (!IsBlocked(x, y + 1, z)) {
                    PushBack((3 << 12) | vertex);
                }
                if (!IsBlocked(x, y, z - 1)) {
                    PushBack((4 << 12) | vertex);
                }
                if (!IsBlocked(x, y, z + 1)) {
                    PushBack((5 << 12) | vertex);
                }
            }
        }
    }
}

Chunk::~Chunk() {
    delete[] voxels_;
    free(vertex_data);
}
