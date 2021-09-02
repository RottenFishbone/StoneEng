#version 460 core
out vec4 FragColor;

uniform sampler2D tex_bitmap_font;

in GS_OUT {
    vec2 v_TexCoord;
    vec3 v_Tint;
} gs_out;


void main() {
    FragColor = texture(tex_bitmap_font, gs_out.v_TexCoord) + vec4(gs_out.v_Tint, 0.0);
}


