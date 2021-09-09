#version 460 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform int tile_size;          // Pixel width of rendered tile
uniform mat4 view_projection;   // View-projection matrix
uniform int atlas_width;        // Total pixel width of atlas texture
uniform int atlas_tile_w;       // Pixel width of atlas tiles

// Output data from this shader (to the frag shader)
out GS_OUT {
    vec2 v_TexCoord;
} gs_out;

// Input data from vert shader
in uint tile_type[];    // The index of the sprite in the atlas

void main() {
    // Offsets to draw a quad around a point
    // Note this goes from corner to corner so the origin is centered.
    // These are used to build a quad via triangle_strip.
    vec4 quad_verts[4] = {
        vec4( -1.0, -1.0, 0.0, 0.0 ), // bottom-left
        vec4( -1.0,  1.0, 0.0, 0.0 ), // top-left
        vec4(  1.0, -1.0, 0.0, 0.0 ), // bottom-right
        vec4(  1.0,  1.0, 0.0, 0.0 ), // top-right
    };
    
    // Calculate a unit uv square using (0, 1) as top left.
    //
    //  y=1 _ _ _ x,y=1
    //     |_|_|_|
    //     |_|_|_|
    //     |_|_|_|
    // x,y=0      x=1
    //
    // We are looking to find the top-left sub-rect in the map. Each sub-rect's
    // width can be calculated as a ratio of the rect size and width of the atlas. 
    // That is, The width of a sprite, atlas_tile_w, divided by the width of the atlas, atlas_width.
    // This 'atlas_ratio' is used to create a square from the top-left, atlas_ratio wide.
    float atlas_ratio = float(atlas_tile_w) / float(atlas_width);
    vec2 unit_uv_verts[4] = {
        vec2( 0.0,         1.0 - atlas_ratio ), // bottom-left
        vec2( 0.0,         1.0               ), // top-left
        vec2( atlas_ratio, 1.0 - atlas_ratio ), // bottom-right
        vec2( atlas_ratio, 1.0               ), // top-right
    };

    // Build a vec4 from the tile_size scalar. 
    // X and Y are equal as the tile is a square.
    vec4 tile_scale = vec4( float(tile_size), float(tile_size), 1.0, 1.0);
    
    // TODO make row_w a uniform to avoid repeated calulations
    int row_w = atlas_width / atlas_tile_w;

    // Calculate an offest to translate a uv unit rect into the correct position.
    // The x offset is calcuated simply as an index -> x coord using modolu
    // The y offset must be made negative as y starts at 1 on the uv rect, thus
    // to advance to row 2, the y pos must be 1 - atlas_ratio.
    vec2 atlas_offset = vec2(  float( tile_type[0] % row_w), 
                              -float( tile_type[0] / row_w));
    for (int i = 0; i < 4; ++i) {
        gl_Position = view_projection * (gl_in[0].gl_Position * tile_scale + quad_verts[i] * (tile_size/2));
        gs_out.v_TexCoord = unit_uv_verts[i] + (atlas_offset * atlas_ratio);
        EmitVertex();
    }
    EndPrimitive();
}


