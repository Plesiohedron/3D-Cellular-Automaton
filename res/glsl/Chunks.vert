#version 460 core

#define VERTEX_COUNT_PER_FACE 4
#define FACE_COUNT_PER_CUBE 6

layout(location = 0) in uint position;

layout(std430, binding = 0) readonly restrict buffer Chunks {
    vec4 models[];
};

layout(std140, binding = 0) uniform Vertices {
    vec4 offsets[VERTEX_COUNT_PER_FACE * FACE_COUNT_PER_CUBE];
};

layout(location = 0) uniform mat4 projview;

layout(location = 0) out vec2 frag_UV;
layout(location = 1) out vec3 frag_color;

void main() {
	// Unpack data
	uint direction = gl_DrawID % 6;
	uint x = (position >> 8);
	uint y = (position >> 4) & 15;
	uint z = (position & 15);
	// Setting UV coordinates for texture
	frag_UV = vec2((gl_VertexID >> 1) & 1, gl_VertexID & 1);
	// Setting brithness of face
	frag_color = vec3(1.0f - 0.1f * direction);
	// Transformation of unpacked data into real vertex's coordinates
	vec3 vertex = vec3(x, y, z) + offsets[(direction << 2) + gl_VertexID].xyz;
	// Setting position of vertex with offset
	gl_Position = projview * vec4(vertex + models[gl_DrawID / 6].xyz, 1.0f);
}