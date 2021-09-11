#pragma once
#include "../../common.h"
#include "../shaders.h"

// Shader paths
#define SHADER_SPRITE_VERT "assets/shaders/sprite_vert.glsl"
#define SHADER_SPRITE_FRAG "assets/shaders/sprite_frag.glsl"
#define SHADER_SPRITE_GEOM "assets/shaders/sprite_geom.glsl"

// The amount of sprites a renderer can hold by default
#define SPRITE_RENDERER_CAPACITY 1024

// TODO Buffer orphaning
// TODO Screen space culling (Quadtrees, likely)
// TODO Sprite Rotation
// TODO Split GPU buffer into static vs dynamically drawn
// TODO **** assign ids to sprites and hold lookup of their pointers.
//      This allows redim/sort with no drawback ****

#define SPRITE_FLAG_INACTIVE        1 << 0
#define SPRITE_FLAG_ANIMATED        1 << 1
#define SPRITE_FLAG_DYNAMIC         1 << 2

/// A textured rectangle to be rendered through the sprite_renderer
/// Max size is a 16tile by 16tile sprite. 
struct sprite {
    /// Note: This must be kept densely packed to avoid stride issues on sending 
    /// data to the GPU.

    /// 3D in-game position of the sprite. (z is depth) (from bottom-left corner).
    fvec3 pos;
    /// RGB color of the sprite. This is applied to the texture.
    fvec3 color;
    /// The degree rotation of the sprite. Breaks pixel-perfection.
    float rotation;
    /// The positional id of the sprite in the spritesheet
    uint16_t sprite_id;
    /// Flags to be set using SPRITE_FLAG_* flags
    uint8_t flags;
    /// right-most 4 bits contain x, left-most contain y. Measured in tiles.
    uint8_t tile_dims;

    /// Animation data
    uint8_t anim_id;    // The id of the animation in vertical order the sheet.
    uint8_t anim_frame; // The current frame on the animation.
    uint8_t anim_total; // The number of frames in the current animation.
    uint8_t anim_wait;  // How many drawn frames to wait before advancing frames.
};


// ----------- [ Sprite ] -------------

/// Builds a 1x1 sprite.
void sprite_build(struct sprite*, fvec3 pos, fvec3 color, float rotation,
                  uint16_t sprite_id, uint8_t flags);
/// Builds a multi-tile sprite. 
void sprite_build_multi(struct sprite*, fvec3 pos, fvec3 color, float rotation,
                        uint16_t sprite_id, uint8_t flags,
                        uint8_t tiles_wide, uint8_t tiles_tall);

/// Frees a sprite struct from memory.
inline void free_sprite(struct sprite*); 
// ------------------------------------


// ---------- [ Renderer ] ------------
/// Builds and initializes a sprite_renderer struct to be used with 
/// sprite_renderer_* functions. The sprite renderer is used to manage and
/// render sprites to the screen by handling all necessary communication with
/// the GPU.
struct sprite_renderer *sprite_renderer_init(fvec2 *camera, mat4 *projection,
                                             const char *sheet_path,
                                             uint32_t sheet_tile_w);

/// Rebuild the sprite_renderer's sprite buffer to a new size. The returned
/// value can be used to offset the newly dangling pointers.
/// ```  
/// struct sprite *s = &r->sprites[0];  
/// int64_t base_offset = sprite_renderer_redim(r, 1024);  
/// s += base_offset;  
/// ```  
/// Will grab the first sprite in the renderer, redim, then redirect the
/// dangling pointer.
int64_t sprite_renderer_redim(struct sprite_renderer*, size_t new_cap);

struct sprite *sprite_renderer_gen(struct sprite_renderer*);

/// Pushes a sprite into the renderer's memory. A pointer to the new sprite
/// memory is returned. The passed sprite can be freed or dropped safely.
struct sprite *sprite_renderer_push(struct sprite_renderer*, struct sprite*);

/// Rebuilds the sprite buffer on GPU and renders to the current draw buffer. 
void sprite_renderer_draw(struct sprite_renderer*);

/// Frees a sprite renderer and all of its sprites from memory.
void free_sprite_renderer(struct sprite_renderer*);
// ------------------------------------
