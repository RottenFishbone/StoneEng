#version 460 core
out vec4 FragColor;

uniform sampler2D tex;

in GS_OUT {
    vec2 v_TexCoord;
} gs_out;

void main() {
    //FragColor = vec4(1.0, 0.5, 0.5, 1.0);
    FragColor = texture(tex, gs_out.v_TexCoord);
}


