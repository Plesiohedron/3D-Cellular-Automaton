#version 460 core

in vec2 frag_UV;
in vec3 frag_color;

out vec4 color;

uniform sampler2D texture0;

void main() {
    color = texture(texture0, frag_UV) * vec4(frag_color, 1.0f);
}