#version 460 core

out vec4 FragColor;

uniform sampler2D tex_atlas;

in GS_OUT {
    vec2 v_TexCoord;
} gs_out;

void main() {
    //FragColor = vec4(1.0, 0.5, 0.5, 1.0);
    FragColor = texture(tex_atlas, gs_out.v_TexCoord);
}


