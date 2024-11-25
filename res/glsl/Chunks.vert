#version 460 core
// I hope your GPU supports at least one of the extensions below
#extension GL_NV_gpu_shader5 : enable
#extension EXT_shader_16bit_storage : enable
#extension GL_AMD_gpu_shader_int16 : enable
#extension GL_EXT_shader_explicit_arithmetic_types : enable
#extension GL_KHR_shader_subgroup_extended_types : enable

layout (location = 0) in uint16_t position;

layout (std430, binding = 0) buffer Offsets {
    vec4 models[];
};

uniform mat4 projview;

out vec2 frag_UV;

void main() {
	// Unpack data
	uint direction = (position >> 12u);
	uint x = (position >> 8u) & 15u;
	uint y = (position >> 4u) & 15u;
	uint z = (position & 15u);
	// Setting UV coordinates for texture
	if (gl_VertexID == 0) {
		frag_UV = vec2(1, 0);
	} else if (gl_VertexID == 2) {
		frag_UV = vec2(1, 1);
	} else if (gl_VertexID == 3) {
		frag_UV = vec2(0, 1);
	} else {
		frag_UV = vec2(0, 0);
	}
	// Transformation unpacked data into real vertex's coordinates
	if (direction == 0) {
		if (gl_VertexID == 0) {
			++y;
		} else if (gl_VertexID == 2) {
			++y; ++z;
		} else if (gl_VertexID == 3) {
			++z;
		}
	} else if (direction == 1) {
		++x;
		if (gl_VertexID == 1) {
			++y;
		} else if (gl_VertexID == 2) {
			++z;
		} else if (gl_VertexID == 3) {
			++y; ++z;
		}
	} else if (direction == 2) {
		if (gl_VertexID == 0) {
			++x;
		} else if (gl_VertexID == 1) {
			++x; ++z;
		} else if (gl_VertexID == 3) {
			++z;
		}
	} else if (direction == 3) {
		++y;
		if (gl_VertexID == 1) {
			++z;
		} else if (gl_VertexID == 2) {
			++x;
		} else if (gl_VertexID == 3) {
			++x; ++z;
		}
	} else if (direction == 4) {
		if (gl_VertexID == 0) {
			++x; ++y;
		} else if (gl_VertexID == 1) {
			++x;
		} else if (gl_VertexID == 2) {
			++y;
		}
	} else {
		++z;
		if (gl_VertexID == 0) {
			++y;
		} else if (gl_VertexID == 2) {
			++x; ++y;
		} else if (gl_VertexID == 3) {
			++x;
		}
	}
	// Setting position of vertex with offset
	gl_Position = projview * vec4(vec3(x, y, z) + models[gl_DrawID].xyz, 1);
}