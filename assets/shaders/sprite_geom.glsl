#version 460 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4 view_projection;
uniform int sheet_width;
uniform int sheet_tile_w;

out GS_OUT {
    vec2 v_TexCoord;
} gs_out;

in uint sprite_id[];
in vec2 sprite_dims[];
in uint anim_id[];
in uint anim_frame[];

void main() {
    // Offsets to draw a quad around a point
    // Note this goes from corner to corner so the origin is centered.
    // These are used to build a quad via triangle_strip.
    vec4 unit_quad_verts[4] = {
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
    // width can be calculated as a ratio of the rect size and width of the sheet. 
    // That is, The width of a sprite, sheet_tile_w, divided by the width of the sheet, sheet_width.
    // This 'sheet_ratio' is used to create a square from the top-left, sheet_ratio wide.
    float sheet_ratio = float(sheet_tile_w) / float(sheet_width);
    vec2 unit_uv_verts[4] = {
        vec2( 0.0,         1.0 - sheet_ratio ), // bottom-left
        vec2( 0.0,         1.0               ), // top-left
        vec2( sheet_ratio, 1.0 - sheet_ratio ), // bottom-right
        vec2( sheet_ratio, 1.0               ), // top-right
    };

    vec2 tile_dims = vec2( float(sheet_tile_w), float(sheet_tile_w));

    // Build 4 new verticies to render a quad
    for (int i = 0; i < 4; ++i) {
        // The sprite origin (middle) passed to the shader
        vec4 point_origin = gl_in[0].gl_Position;
        // The offset of this vertex from origin, used to draw the unit quad
        vec4 point_offset = unit_quad_verts[i] * vec4(tile_dims/2.0, 0.0, 0.0);
        // The scale vector for the quad to span the needed sprite dimensions
        vec4 quad_scale = vec4(sprite_dims[0], 1.0, 1.0);
        // The translation vector to move it back to origin.
        // particular translation will result in the bottom left tile being the anchor
        // for the scale.
        vec4 quad_transl = vec4((sprite_dims[0].x-1.0) * tile_dims.x/2.0,
                                (sprite_dims[0].y-1.0) * tile_dims.y/2.0, 0.0, 0.0);
        
        // Finally calculate the vertex position.
        gl_Position = view_projection * (point_origin + point_offset * quad_scale + quad_transl);

        // TODO stretch unit verts and translate to the correct place on
        //      on the sheet based on sprite_dims
        gs_out.v_TexCoord = unit_uv_verts[i];
        EmitVertex();
    }
    EndPrimitive();
    
}
