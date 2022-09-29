#version 450 core

/* Inputs */
layout (location = 0) in vec3 i_Position;
layout (location = 1) in vec4 i_Color;

layout (location = 2) in uint i_TexIndex;
layout (location = 3) in vec2 i_TexCoord;

/* Outputs */
layout (location = 0) out vec4 o_Color;
layout (location = 1) flat out uint o_TexIndex;
layout (location = 2) out vec2 o_TexCoord;

void main() {
    // Fill outputs
    o_Color = i_Color;
    o_TexIndex = i_TexIndex;
    o_TexCoord = i_TexCoord;

    gl_Position = vec4(i_Position, 1.0f);
}