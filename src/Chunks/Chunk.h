#pragma once

#include <glm/glm.hpp>

using Vertex = uint16_t;
using Voxel = uint8_t;

class Chunk {
    friend class Chunks;

public:
    static const int DIRECTION_SIZE = 16;

    static const int WIDTH = DIRECTION_SIZE;
    static const int HEIGHT = DIRECTION_SIZE;
    static const int DEPTH = DIRECTION_SIZE;

    static const int AREA = DIRECTION_SIZE * DIRECTION_SIZE;
    static const int VOLUME = WIDTH * HEIGHT * DEPTH;

    static const int BASE_VERTEX_DATA_CAPACITY = 256;

    glm::ivec3 global_coordinates;  // real coordinates in the universe
    glm::ivec3 local_coordinates;

    size_t vertex_data_size = 0;
    size_t vertex_data_capacity = BASE_VERTEX_DATA_CAPACITY;
    Vertex* vertex_data;

    bool is_modified = true;
    bool is_visible = true;

private:
    Voxel* voxels_;

private:
    bool IsBlocked(int x, int y, int z) const;
    void PushBack(Vertex vertex);

    void Mesh();

private:
    Chunk(const glm::ivec3& coordinates, const glm::ivec3& chunks_sizes);
    Chunk(const Chunk&) = delete;
    ~Chunk();
};
