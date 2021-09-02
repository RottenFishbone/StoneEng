#pragma once
#include "../../common.h"
#include "../shaders.h"

#define SHADER_TILE_VERT "assets/shaders/tile_vert.glsl"
#define SHADER_TILE_FRAG "assets/shaders/tile_frag.glsl"
#define SHADER_TILE_GEOM "assets/shaders/tile_geom.glsl"
#define ATLAS_IMAGE_PATH "assets/textures/atlas.png"

struct render_tile {
    uint32_t local_id;      // Id of tile within the chunk
    uint8_t  type_id;       // Texture/Tile type Id
    uint8_t  bitmask;       // Determine's which connection tile to use
    uint8_t  flags;         // Passes boolean parameters
    uint8_t  padding;       // Unused
};

// void set_atlas(..){}
struct tile_renderer *tile_renderer_init(fvec2 *camera_inst, mat4 *projection);
void free_tile_renderer(struct tile_renderer *renderer);
void tile_renderer_draw(struct tile_renderer *r);

void tile_renderer_set_tile(struct tile_renderer*, uint8_t type_id, vec2 pos); 
// TODO Change to point_sprites maybe? perhaps faster performance.
