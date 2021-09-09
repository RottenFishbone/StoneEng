#pragma once
#include "../../common.h"
#include "../shaders.h"

#define SHADER_SPRITE_VERT "assets/shaders/sprite_vert.glsl"
#define SHADER_SPRITE_FRAG "assets/shaders/sprite_frag.glsl"
#define SHADER_SPRITE_GEOM "assets/shaders/sprite_geom.glsl"

/// The amount of sprites to increase the sprite_renderer's buffer capacity
/// by whenever exceeded.
#define SPRITE_RENDERER_BUFF_INCR 1024

// TODO Buffer orphaning
// TODO Screen space culling (Quadtrees, likely)
// TODO Sprite Rotation
// TODO Split GPU buffer into static vs dynamically drawn
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

inline void free_sprite(struct sprite*); 

// ------------------------------------


// ---------- [ Renderer ] ------------

/// Builds and initializes a sprite_renderer struct to be used with 
/// sprite_renderer_* functions. The sprite renderer is used to manage and
/// render sprites to the screen by handling all necessary communication with
/// the GPU.
struct sprite_renderer *sprite_renderer_init(mat4 *projection, fvec2 *camera, 
                                             const char *atlas_path);

struct sprite *sprite_renderer_push(struct sprite_renderer*, struct sprite*);

/// Renders all active sprites to the current buffer.
void sprite_renderer_draw(struct sprite_renderer*);

inline void free_sprite_renderer(struct sprite_renderer*);

// ------------------------------------
