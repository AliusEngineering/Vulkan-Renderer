#version 450 core

/* Inputs */
layout (location = 0) in vec4 i_Color;
layout (location = 1) flat in uint i_TexIndex;
layout (location = 2) in vec2 i_TexCoord;

// Temporarily disabled
// layout (binding = 0) uniform sampler2D u_Textures[32];

/* Outputs */
layout (location = 0) out vec4 o_Color;

void main() {
    vec4 Color;

    if (i_TexIndex == 0) {
        Color = i_Color;
    }
    // Temporarily disabled
    //    else {
    //        Color = texture(u_Textures[i_Input.TexIndex], i_Input.TexCoord);
    //    }

    o_Color = Color;
}