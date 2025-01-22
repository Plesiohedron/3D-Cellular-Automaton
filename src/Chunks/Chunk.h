#pragma once

#include <glm/glm.hpp>

using Vertex = uint16_t;
using Voxel = uint8_t;

class Chunks;

class Chunk {
    friend class Chunks;

public:
    static constexpr int DIRECTION_SIZE = 16;

    static constexpr int WIDTH = DIRECTION_SIZE;
    static constexpr int HEIGHT = DIRECTION_SIZE;
    static constexpr int DEPTH = DIRECTION_SIZE;

    static constexpr int AREA = DIRECTION_SIZE * DIRECTION_SIZE;
    static constexpr int VOLUME = WIDTH * HEIGHT * DEPTH;

    static constexpr int BASE_VERTEX_DATA_CAPACITY = 16;

    static constexpr int VERTEX_COUNT_PER_FACE = 4;
    static constexpr int FACE_COUNT_PER_CUBE = 6;

    glm::ivec3 global_coordinates;  // real coordinates in the universe
    glm::ivec3 local_coordinates;

    size_t vertex_data_size[FACE_COUNT_PER_CUBE];
    size_t vertex_data_capacity[FACE_COUNT_PER_CUBE];
    Vertex* vertex_data[FACE_COUNT_PER_CUBE]{nullptr};

    bool is_modified = false;
    bool is_visible = false;
    bool previously_is_visible = false;

private:
    Voxel* voxels_ = nullptr;
    const Chunks* chunks_ = nullptr;

private:
    bool IsBlocked(int x, int y, int z) const;
    void PushBack(Vertex vertex, int index);

    void Mesh();

private:
    Chunk(const glm::ivec3& coordinates, const glm::ivec3& chunks_sizes, const Chunks* chunks);
    Chunk(const Chunk&) = delete;
    ~Chunk();
};
