#include "../../../include/gfx/renderer/sprite_renderer.h"

void sprite_build(struct sprite *s, fvec3 pos, fvec3 color, float rotation,
                   uint16_t sprite_id, uint8_t flags){
    sprite_build_multi(s, pos, color, rotation, sprite_id, flags, 1, 1);
}
void sprite_build_multi(struct sprite *s, 
                        fvec3 pos, fvec3 color, float rotation,
                        uint16_t sprite_id, uint8_t flags,
                        uint8_t tiles_wide, uint8_t tiles_tall){
    
    if (tiles_wide || tiles_tall > 16) {
        ZF_LOGE("[sprite_build_multi] Building a sprite more than 16 tall or \
                wide will result in overflow.\n");
    }

    *s = (struct sprite) {
        .pos = pos,
        .color = color,
        .rotation = rotation,
        .sprite_id = sprite_id,
        .flags = flags,
        .tile_dims = (tiles_wide-1) | ((tiles_tall-1) << 4),
    
        .anim_id = 0,
        .anim_frame = 0,
        .anim_total = 0,
        .anim_wait = 0,
    }; 
}

extern inline void free_sprite(struct sprite *s) { free(s); }

struct sprite_renderer {
    // Configs
    const char *atlas_path; 
    uint32_t atlas_sprite_width;
    
    // Data
    struct sprite *sprites;
    size_t sprites_count;
    size_t buffer_capacity;

    // OpenGL vars 
    GLuint program_id;
    GLuint vert_array;
    GLuint buffers[1];
    GLuint uniform_locs[1];
    GLuint texture;

    // View/Projections
    mat4 *projection;
    fvec2 *camera;

};

struct sprite_renderer *sprite_renderer_init(mat4 *projection, 
                                             fvec2 *camera, 
                                             const char *atlas_path){
    
    // Initialize fields
    struct sprite_renderer *r = checked_malloc(sizeof(struct sprite_renderer));
    r->projection = projection;
    r->camera = camera;
    r->atlas_path = atlas_path;
    r->sprites_count = 0;
    r->buffer_capacity = SPRITE_RENDERER_BUFF_INCR;
    r->sprites = checked_malloc(sizeof(struct sprite) * r->buffer_capacity);
   
    return r;
}

Diag_IgnoreUnused
struct sprite **sprite_renderer_load_batch(struct sprite_renderer *r, 
                                           struct sprite *s, 
                                           size_t count){
    todo();
    return NULL;
}

struct sprite *sprite_renderer_load(struct sprite_renderer *r, struct sprite *s){
    todo();
    return NULL;
}

void sprite_renderer_draw(struct sprite_renderer *r){
    todo();
}
Diag_Pop



extern inline void free_sprite_renderer(struct sprite_renderer *r){
    free(r->sprites);
    free(r);
}
