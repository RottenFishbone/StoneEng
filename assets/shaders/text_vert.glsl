#version 460 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec3 font_color_in;
layout (location = 2) in float font_size_in;
layout (location = 3) in int ascii_val_in;

uniform mat4 projection;

out vec3 font_color;
out float font_size;
out int ascii_val;

void main() {
    gl_Position = vec4(pos, 0.0, 1.0);
    
    ascii_val = ascii_val_in - 32;
    font_color = font_color_in;
    font_size = font_size_in;
}


