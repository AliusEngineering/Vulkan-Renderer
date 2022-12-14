#version 450 core

vec2 positions[6] = vec2[](
vec2(-0.5, 0.5), vec2(-0.5, -0.5), vec2(0.5, 0.5),
vec2(0.5, -0.5), vec2(-0.5, -0.5), vec2(0.5, 0.5)
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 1.0, 1.0);
}