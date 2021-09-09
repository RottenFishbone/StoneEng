#include "../../../include/gfx/renderer/text_renderer.h"
#include "../../../include/stb_image.h"

// TODO refactor this garbage.

#define TEXT_DATA_SCREENSPACE 0x1
/// Used to package data for an OpenGL array buffer.
struct text_data {
    fvec2 position;   //  8b
    fvec3 font_color; // 12b
    float font_size;  //  4b
    char character;   //  1b
                      //  3b (padding)
};

struct text_renderer {
    // Data
    struct text_block **text_blocks;
    size_t blocks_len;
    size_t data_len;

    // OpenGL vars 
    GLuint program_id;
    GLuint vert_array;
    GLuint buffers[1];
    GLuint uniform_locs[1];
    GLuint texture;

    // Matricies
    mat4 *projection;
};

void free_text_renderer(struct text_renderer *r){
    // Free up all the text blocks
    if (r->text_blocks) { 
        // Free the contents of each block
        for (size_t i = 0; i < r->blocks_len; ++i){ 
            if (r->text_blocks[i]->contents){
                free(r->text_blocks[i]->contents);
            }
            free(r->text_blocks[i]);
        }
        // Free the block itself
        free(r->text_blocks); 
    }

    // Free the renderer itself
    free(r); 
}

struct text_block *text_renderer_new_block(struct text_renderer *r) {
    // Allocate enough memory for the new block
    struct text_block **blocks = checked_malloc(sizeof(struct text_block*) * ++r->blocks_len);
    struct text_block *new_block = checked_malloc(sizeof(struct text_block));
    *new_block = (struct text_block) {
        .contents = NULL,
        .length = 0,
        .font_size = 1.f,
        .font_color = { .x = 1.f, .y = 1.f, .z = 1.f },
        .anchor_pos = { .x = 0.f, .y = 0.f },
    };
    blocks[r->blocks_len-1] = new_block;

    // Clone the previous r->text_blocks into the new memory and free the old one
    if (r->text_blocks){
        memcpy(blocks, r->text_blocks, sizeof(struct text_block*) * (r->blocks_len-1));
        free(r->text_blocks);
    }
    
    r->text_blocks = blocks;
    return new_block;
}

void text_block_edit_contents(struct text_block *b, char *contents) {
    if (contents) {
        size_t len = strlen(contents)+1; 
        char *text = checked_malloc(sizeof(char) * len);
        memcpy(text, contents, sizeof(char) * len);
        b->contents = text;
        b->length = len-1;
    }
}

Diag_IgnoreUnused
void text_renderer_remove_block(struct text_renderer *r, struct text_block *b){
    // TODO implement
    todo();
}
Diag_Pop

void text_renderer_update(struct text_renderer *r){
    const float LETTER_SIZE = 8.f;
    // Calculate size of data buffer needed
    r->data_len = 0;
    for (size_t block_i = 0; block_i < r->blocks_len; ++block_i) {
        r->data_len += r->text_blocks[block_i]->length;
    }
    // Allocate the renderable text data
    struct text_data *data = checked_malloc(sizeof(struct text_data) * r->data_len); 
    size_t data_i = 0;
    // Loop through the text blocks, converting each one to renderable data
    for (size_t block_i = 0; block_i < r->blocks_len; ++block_i){
        struct text_block *block = r->text_blocks[block_i];
        fvec2 anchor_start = block->anchor_pos;
        
        fvec2 cur_pos = block->anchor_pos;
        for (size_t text_i = 0; text_i < block->length; ++text_i) {
            if (block->contents[text_i] == '\n') {
                cur_pos.x = anchor_start.x;
                cur_pos.y -= LETTER_SIZE * block->font_size;
                continue;
            }
            data[data_i].character = block->contents[text_i];
            data[data_i].position = cur_pos;
            data[data_i].font_size = block->font_size;
            data[data_i].font_color = block->font_color;
            cur_pos.x += LETTER_SIZE * block->font_size;
            
            ++data_i;
        }
    }
    // Send text data to GPU
    glUseProgram(r->program_id);
    glBindVertexArray(r->vert_array);
    glBindBuffer(GL_ARRAY_BUFFER, r->buffers[0]);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(struct text_data) * r->data_len, data, GL_STATIC_DRAW);
    
    // Position
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof(struct text_data), 
                           (GLvoid*) 0);
    // Font color
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, sizeof(struct text_data),
                           (GLvoid*) sizeof(fvec2));
    // Font size
    glVertexAttribPointer( 2, 1, GL_FLOAT, GL_FALSE, sizeof(struct text_data), 
                           (GLvoid*) (sizeof(fvec2) + sizeof(fvec3)));
    // Character
    glVertexAttribIPointer(3, 1, GL_BYTE, sizeof(struct text_data), 
                           (GLvoid*) (sizeof(fvec2) + sizeof(fvec3) + sizeof(GLfloat)));
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
    glUseProgram(0);
}

struct text_renderer *text_renderer_init(mat4 *projection) {
    // Allocate a memory for text_renderer
    struct text_renderer *r = checked_malloc(sizeof(struct text_renderer));
    
    // Init text_renderer fields
    r->blocks_len = 0;
    r->data_len = 0;
    r->text_blocks = NULL;
    r->projection = projection;

    if (!shader_program_from_files(&r->program_id, SHADER_TEXT_VERT, 
                                   SHADER_TEXT_FRAG, SHADER_TEXT_GEOM)){
        ZF_LOGE("Failed to create text_renderer shaders.");
        goto err;
    }

    // Bind and generate required OpenGL buffers and arrays
    glUseProgram(r->program_id);
    glGenVertexArrays(1, &r->vert_array);
    glGenBuffers(1, r->buffers);
    glGenTextures(1, &r->texture);
    
    // Bind our target vertex array
    glBindVertexArray(r->vert_array);
    glBindTexture(GL_TEXTURE_2D, r->texture);

    // Set texture options, in this case aiming towards pixel precision, not smoothing
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    // Load bitmap font using STBI
    int w,h,channels;
    stbi_set_flip_vertically_on_load(1);
    unsigned char *img_data = stbi_load(FONT_IMG_PATH, &w, &h, &channels, STBI_rgb_alpha);
    if (!img_data) {
        ZF_LOGE("Failed to load font image: %s\n", FONT_IMG_PATH);
        goto err;
    }
    // Store image data as a texture on the GPU and drop from host memory
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data);
    stbi_image_free(img_data);
   
    // Load the uniform locations once and cache to save time
    r-> uniform_locs[0] = glGetUniformLocation(r->program_id, "projection");

    // Unbind OpenGL objects
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);

    return r;
err:
    return NULL;
}

void text_renderer_draw(struct text_renderer *r) {
    if (!r->blocks_len) { return; }

    glUseProgram(r->program_id);
    glBindVertexArray(r->vert_array);
    glBindTexture(GL_TEXTURE_2D, r->texture);

    glUniformMatrix4fv(r->uniform_locs[0], 1, GL_FALSE, r->projection->m);

    glDrawArrays(GL_POINTS, 0, r->data_len);

    glBindVertexArray(0);
    glUseProgram(0);
}

