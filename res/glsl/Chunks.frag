#version 460 core

in vec2 frag_UV;

out vec4 color;

uniform sampler2D texture0;

void main() {
    color = texture(texture0, frag_UV);
}