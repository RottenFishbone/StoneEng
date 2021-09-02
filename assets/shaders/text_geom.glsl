#version 460 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out; 

uniform mat4 projection;

// Output data from this shader (to the frag shader)
out GS_OUT {
    vec2 v_TexCoord;
    vec3 v_Tint;
} gs_out;

in vec3 font_color[];
in float font_size[];
in int ascii_val[];

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
    float atlas_res = 8; float atlas_size = 128;
    float atlas_scale = atlas_res / atlas_size;
    vec2 unit_uv_coords[4] = {
        vec2( 0.0, 1.0 - atlas_scale ),         // bottom-left
        vec2( 0.0, 1.0 ),                       // top-left
        vec2( atlas_scale,   1.0 - atlas_scale),// bottom-right
        vec2( atlas_scale,   1.0 ),             // top-right
    };

    // Calculate the translation of the unit_uv_coords,
    // As the every x increment is scale wide starting from 0, its simply x * scale,
    // whereas y is starting at 1 and every increment reduces by scale, i.e. y * -scale
    int atlas_per_row = 16;
    vec2 atlas_offset = vec2( float(ascii_val[0] % atlas_per_row), -float( ascii_val[0] / atlas_per_row) );
    for (int i = 0; i < 4; ++i) {
        vec4 vert_offset = quad_verts[i] * (atlas_res / 2.0) * font_size[0];
        gl_Position = projection * (gl_in[0].gl_Position + vert_offset);
        gs_out.v_TexCoord = unit_uv_coords[i] + atlas_offset * atlas_scale;
        gs_out.v_Tint = font_color[0];
        EmitVertex();
    }
    EndPrimitive();
}


