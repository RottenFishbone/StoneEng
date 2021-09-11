#version 460 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in float rotation;

// Sprite data must be packed as:
// [ 0x00 0x00 0x00 0x00 ]
//   |--| |--| |-------|
//   dims flag sprite_id
// Where:
//  id     - The location of the idle sprite. This is determined from 
//           right to left, from the top-left corner = 0:
//              0 1 2 3
//              4 5 6 7
//              ...
//
//  flag  - Flags applying directly to this sprite
//  dims  - How many tiles wide and tall the sprite is with the right half
//          of the byte being x and the left being y. Zero defaults to 1x1.
//         
layout (location = 3) in uint sprite_data;

// Animation data must be packed as:
// [ 0x00  0x00  0x00  0x00 ]
//   |--------|  |--|  |--|
//     unused    frame  id
// Where:
//  id    - The index of the animation, arranged vertically in the spritesheet
//          0 is the idle state.
//  frame - The current frame of the animation, from left = 0 to right = total
//  
layout (location = 4) in uint anim_data;

uniform mat4 view_projection;
uniform int sheet_width;
uniform int sheet_tile_w;

out uint sprite_id;
out vec2 sprite_dims;
out uint anim_id;
out uint anim_frame;

void main() {
    // Unpack sprite data
    sprite_id   = sprite_data & 0xFF;
    sprite_dims = vec2(float((sprite_data >> 24) & 0xF) + 1.0,
                       float((sprite_data >> 28) & 0xF) + 1.0);
    
    // Unpack animation data
    anim_id    =  anim_data & 0xF;
    anim_frame = (anim_data >> 8) & 0xF;
    
    // Forward position to geometry shader
    gl_Position = vec4(position, 1.0);
    
    // TODO rotation
}
