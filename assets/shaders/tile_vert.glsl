#version 460 core
layout (location = 0) in uint tile_id;
layout (location = 1) in uint data;
uniform mat4 view_projection;
uniform int tile_size;
uniform int chunk_size;
uniform int atlas_width;
uniform int atlas_tile_w;

out uint tile_type;

void main() {
    float x = float(tile_id % chunk_size);
    float y = float(tile_id / chunk_size);
    gl_Position = vec4(x, y, 0.1, 1.0);
    
    tile_type    = (data     ) & 0xFF;
    uint bitmask = (data >> 8) & 0xFF;
}


