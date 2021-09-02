#include "../../../include/gfx/renderer/tile_renderer.h"
#include "../../../include/stb_image.h"

struct tile_renderer {
    fvec2 *camera;
    struct render_tile map[CHUNK_AREA];
    
    // OpenGL vars 
    GLuint program_id;
    GLuint vert_array;
    GLuint buffers[1];
    GLuint uniform_locs[6];
    GLuint texture;

    mat4 *projection;
};


void free_tile_renderer(struct tile_renderer *renderer){
    free(renderer);
}

struct tile_renderer *tile_renderer_init(fvec2 *camera_inst, mat4 *projection){
    
    struct tile_renderer *tr = checked_malloc(sizeof(struct tile_renderer));
    tr->camera = camera_inst;
    tr->projection = projection;
    memset(tr->map, 0, sizeof(tr->map));    

    for (size_t i = 0; i < CHUNK_AREA; ++i){
        tr->map[i].local_id = i;
        tr->map[i].type_id = rand()%2;
    }

    if (!shader_program_from_files(&tr->program_id, SHADER_TILE_VERT, 
                                   SHADER_TILE_FRAG,SHADER_TILE_GEOM)){
        ZF_LOGE("Failed to link tile renderer program.\n");
        goto err;
    }
    
    // Create OpenGL buffers and objects
    glUseProgram(tr->program_id);
    
    // Generate OpenGL objects
    glGenVertexArrays(1, &tr->vert_array);
    glGenBuffers(1, tr->buffers);
    glGenTextures(1, &tr->texture);
   
    glBindVertexArray(tr->vert_array);
    glBindTexture(GL_TEXTURE_2D, tr->texture);

    // Set texture options ( GL_NEAREST maintaining a pixel style )
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Loading texture atlas into memory
    int w,h,channels;
    stbi_set_flip_vertically_on_load(1);
    unsigned char *data = stbi_load(ATLAS_IMAGE_PATH, &w, &h, &channels, STBI_rgb_alpha);
    if (!data) {
        ZF_LOGE("Failed to load atlas file: %s\n", ATLAS_IMAGE_PATH);
        goto err;
    }
    // Load atlas into GPU texture buffer
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);      // Image data is on GPU, free the host memory.

    // Bind and allocate the vertex data buffer
    glBindBuffer(GL_ARRAY_BUFFER, tr->buffers[0]);
    glBufferStorage(GL_ARRAY_BUFFER, sizeof(struct render_tile) * CHUNK_AREA, tr->map, 
                    GL_MAP_WRITE_BIT | GL_DYNAMIC_STORAGE_BIT);
    
    // local_id attribute, where the tile is to be rendered in a chunk
    glVertexAttribIPointer(0, 1, GL_UNSIGNED_INT, 2 * sizeof(GLuint), (GLvoid*)0);
    glEnableVertexAttribArray(0); 
    // tile_data attribute, 32 bits of tile data to be decoded on the GPU
    glVertexAttribIPointer(1, 1, GL_UNSIGNED_INT, 2 * sizeof(GLuint), (GLvoid*)(sizeof(GLuint)));
    glEnableVertexAttribArray(1);


    // Get uniform locations from GPU
    tr->uniform_locs[0] = glGetUniformLocation(tr->program_id, "mvp"); 
    tr->uniform_locs[1] = glGetUniformLocation(tr->program_id, "tile_size"); 
    tr->uniform_locs[2] = glGetUniformLocation(tr->program_id, "chunk_size");
    tr->uniform_locs[3] = glGetUniformLocation(tr->program_id, "atlas_size");
    tr->uniform_locs[4] = glGetUniformLocation(tr->program_id, "atlas_res");

    // Unbind
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
    return tr;

err:
    return NULL;
}

Diag_IgnoreUnused
int tile_renderer_queue_change(struct tile_renderer *r, struct render_tile change){
    // TODO complete
    todo();
    return 0;
}
Diag_Pop

void tile_renderer_set_tile(struct tile_renderer *r, uint8_t type_id, vec2 pos){
    size_t index = pos.x + pos.y * CHUNK_WIDTH;
    glUseProgram(r->program_id);
    glBindVertexArray(r->vert_array);
    glBindBuffer(GL_ARRAY_BUFFER, r->buffers[0]);

    size_t offset = sizeof(struct render_tile) * index;
    r->map[index].type_id = type_id;
    glBufferSubData(GL_ARRAY_BUFFER, offset, 
                    sizeof(struct render_tile), (GLvoid*) &r->map[index]);

    glBindVertexArray(0);
    glUseProgram(0);
}

void tile_renderer_draw(struct tile_renderer *r) {

    // Bind OpenGL objects
    glUseProgram(r->program_id);
    glBindVertexArray(r->vert_array);
    glBindTexture(GL_TEXTURE_2D, r->texture);
    
    // Generate view-projection matrix
    mat4 view = lalg_translation(r->camera->x, r->camera->y, 0.0);
    mat4 vp = lalg_cross(&view, r->projection);
    // Load view-projection matrix into kernel 
    glUniformMatrix4fv(r->uniform_locs[0], 1, GL_FALSE, vp.m);

    glUniform1i(r->uniform_locs[1], 32);
    glUniform1i(r->uniform_locs[2], CHUNK_WIDTH);
    glUniform1i(r->uniform_locs[3], 256);
    glUniform1i(r->uniform_locs[4], 32);

    glDrawArrays(GL_POINTS, 0, CHUNK_AREA);

    glBindVertexArray(0);
    glUseProgram(0);
}
