#include "../../../include/gfx/renderer/sprite_renderer.h"
#include "../../../include/stb_image.h"

// --------- [ Sprite ] -----------
void sprite_build(struct sprite *s, fvec3 pos, fvec3 color, float rotation,
                  uint16_t sprite_id, uint8_t flags){
    // Pass the parameters to build_multi with 1x1 tile 
    sprite_build_multi(s, pos, color, rotation, sprite_id, flags, 1, 1);
}
void sprite_build_multi(struct sprite *s, fvec3 pos, fvec3 color, float rotation,
                        uint16_t sprite_id, uint8_t flags,
                        uint8_t tiles_wide, uint8_t tiles_tall){
    
    if (tiles_wide > 16 || tiles_tall > 16) {
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
// ---------------------------------------------

// --------- [ Sprite Renderer ] -----------
struct sprite_renderer {
    // Configs
    const char *sheet_path; 
    uint32_t sheet_width;
    uint32_t sheet_tile_w;
    // Data
    struct sprite *sprites;
    size_t sprites_count;
    size_t sprites_capacity;

    // OpenGL vars 
    GLuint program_id;
    GLuint vert_array;
    GLuint buffers[1];
    GLuint uniform_locs[3];
    GLuint texture;

    // View/Projections
    mat4 *projection;
    fvec2 *camera;

};

struct sprite_renderer *sprite_renderer_init(fvec2 *camera, mat4 *projection,
                                             const char *sheet_path,
                                             uint32_t sheet_tile_w){
    
    // Initialize fields
    struct sprite_renderer *r = checked_malloc(sizeof(struct sprite_renderer));
    r->projection = projection;
    r->camera = camera;

    r->sheet_path = sheet_path;
    r->sheet_tile_w = sheet_tile_w;
    
    r->sprites_count = 0;
    r->sprites_capacity = SPRITE_RENDERER_CAPACITY;
    r->sprites = checked_malloc(sizeof(struct sprite) * r->sprites_capacity);

    if (!shader_program_from_files(&r->program_id, SHADER_SPRITE_VERT, 
                              SHADER_SPRITE_FRAG, SHADER_SPRITE_GEOM)) {
        ZF_LOGE("[sprite_renderer] Failed to link OpenGL program.");
        exit(EXIT_FAILURE);
    }
    
    // Generate OpenGL buffers
    glGenVertexArrays(1, &r->vert_array);
    glGenBuffers(1, r->buffers);
    glGenTextures(1, &r->texture);
 
    // Bind OpenGL states/buffers
    glUseProgram(r->program_id);
    glBindVertexArray(r->vert_array);
    glBindBuffer(GL_ARRAY_BUFFER, r->buffers[0]);
    glBindTexture(GL_TEXTURE_2D, r->texture);

    // Set texture options ( GL_NEAREST maintaining a pixel style )
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Loading image into memory
    int w,h,channels;
    stbi_set_flip_vertically_on_load(1);
    unsigned char *data = stbi_load(r->sheet_path, &w, &h, &channels, STBI_rgb_alpha);
    if (!data) {
        ZF_LOGE("Failed to load spritesheet file: %s\n", r->sheet_path);
        exit(EXIT_FAILURE);
    }
    // Send the image data to the GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);      // Image data is on GPU, free the host memory. 
    r->sheet_width = w;

    // Setup attribute pointers
    // Pos
    size_t stride = sizeof(struct sprite);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*)0); 
    glEnableVertexAttribArray(0);
    // Color
    size_t color_offset = sizeof(fvec3);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (GLvoid*) color_offset); 
    glEnableVertexAttribArray(1);
    // Rotation
    size_t rotation_offset = color_offset + sizeof(fvec3); 
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (GLvoid*) rotation_offset); 
    glEnableVertexAttribArray(2);
    // Sprite data (sprite_id, flags, tile_dims)
    size_t spr_data_offset = rotation_offset + sizeof(float);
    glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, stride, (GLvoid*) spr_data_offset); 
    glEnableVertexAttribArray(3);
    // Anim data (anim_id, anim_frame, anim_total, anim_wait);
    size_t anim_data_offset = spr_data_offset + sizeof(uint32_t);
    glVertexAttribIPointer(4, 1, GL_UNSIGNED_INT, stride, (GLvoid*) anim_data_offset); 
    glEnableVertexAttribArray(4);

    // Store uniform locations
    r->uniform_locs[0] = glGetUniformLocation(r->program_id, "view_projection");
    r->uniform_locs[1] = glGetUniformLocation(r->program_id, "sheet_width");
    r->uniform_locs[2] = glGetUniformLocation(r->program_id, "sheet_tile_w");

    // Unbind OpenGL states/buffers
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0); 
    return r;
}

void sprite_renderer_draw(struct sprite_renderer *r){
    // Bind GL
    glUseProgram(r->program_id);
    glBindVertexArray(r->vert_array);
    glBindTexture(GL_TEXTURE_2D, r->texture);
    
    // Rebuild view projection for this draw
    mat4 view = lalg_translation(r->camera->x, r->camera->y, 0.0);
    mat4 view_proj = lalg_cross(&view, r->projection);
    glUniformMatrix4fv(r->uniform_locs[0], 1, GL_FALSE, view_proj.m);

    // Load uniforms
    glUniform1i(r->uniform_locs[1], r->sheet_width);
    glUniform1i(r->uniform_locs[2], r->sheet_tile_w);

    // Push sprite data to GPU
    glBindBuffer(GL_ARRAY_BUFFER, r->buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(struct sprite) * r->sprites_count,
                 r->sprites, GL_DYNAMIC_DRAW);

    glDrawArrays(GL_POINTS, 0, r->sprites_count);
    // Unbind GL
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

struct sprite *sprite_renderer_gen(struct sprite_renderer *r){
    if (r->sprites_count == r->sprites_capacity){
        ZF_LOGE("[sprite_renderer] Exceeding sprite renderer capacity. \
Use sprite_renderer_redim() to expand the sprite buffer.");
    }
    return &r->sprites[r->sprites_count++];
}

struct sprite *sprite_renderer_push(struct sprite_renderer *r, struct sprite *s){
    if (r->sprites_count == r->sprites_capacity){
        ZF_LOGE("[sprite_renderer] Exceeding sprite renderer capacity.\
 Use sprite_renderer_redim() to expand the sprite buffer.");
    }

    // Clone passed sprite into renderer memory
    struct sprite *rsprite = &r->sprites[r->sprites_count++];
    memcpy(rsprite, s, sizeof(struct sprite)); 
    
    // Returns pointer to new sprite in renderer memory
    return rsprite;
}

Diag_IgnoreUnused
int64_t sprite_renderer_redim(struct sprite_renderer *r, size_t new_cap){
    todo();
    return 0;
}
Diag_Pop

void free_sprite_renderer(struct sprite_renderer *r){
    if (r->sprites){
        free(r->sprites);
    }
    if (r){
        free(r);
    }
}
