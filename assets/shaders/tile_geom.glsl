#version 460 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform int tile_size;   // Pixel width of rendered tile
uniform mat4 mvp;        // Model-view-projection matrix
uniform int atlas_size;  // Total pixel width of atlas texture
uniform int atlas_res;   // Pixel width of atlas tiles

// Output data from this shader (to the frag shader)
out GS_OUT {
    vec2 v_TexCoord;
} gs_out;

in uint tile_type[];

void main() {
    // Offsets to draw a quad around a point
    vec4 quad_verts[4] = {
        vec4( -1.0, -1.0, 0.0, 0.0 ), // bottom-left
        vec4( -1.0,  1.0, 0.0, 0.0 ), // top-left
        vec4(  1.0, -1.0, 0.0, 0.0 ), // bottom-right
        vec4(  1.0,  1.0, 0.0, 0.0 ), // top-right
    };
    
    // Calculate uv_coords using (0, 1) as top left.
    // The target texture is 'scale' wide, thus, the bottom
    // is calculated as ( 1.0 - scale ) for the top-left tile's bottom
    // atlas_scale is calculated as ratio of each textures size to the atlas width
    float atlas_scale = float(atlas_res) / float(atlas_size);
    vec2 unit_uv_coords[4] = {
        vec2( 0.0, 1.0 - atlas_scale ),   // bottom-left
        vec2( 0.0, 1.0 ),       // top-left
        vec2( atlas_scale,   1.0 - atlas_scale),        // bottom-right
        vec2( atlas_scale,   1.0 ),           // top-right
    };

    
    // Calculate the translation of the unit_uv_coords,
    // As the every x increment is scale wide starting from 0, its simply x * scale,
    // whereas y is starting at 1 and every increment reduces by scale, i.e. y * -scale
    vec4 tile_scale = vec4( float(tile_size), float(tile_size), 1.0, 1.0);

    // TODO allow for 2d offsets
    vec2 atlas_offset = vec2( float(tile_type[0]), 0.0 );
    for (int i = 0; i < 4; ++i) {
        gl_Position = mvp * (gl_in[0].gl_Position * tile_scale + quad_verts[i] * (tile_size/2));
        gs_out.v_TexCoord = unit_uv_coords[i] + atlas_offset * atlas_scale;
        EmitVertex();
    }
    EndPrimitive();
}


