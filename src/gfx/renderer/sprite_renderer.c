#include "../../../include/gfx/renderer/sprite_renderer.h"

struct sprite_renderer {
    // Data

    // OpenGL vars 
    GLuint program_id;
    GLuint vert_array;
    GLuint buffers[1];
    GLuint uniform_locs[1];
    GLuint texture;

    // Matricies
    mat4 *projection;
};

struct sprite_renderer *sprite_renderer_init(mat4 *projection){
    struct sprite_renderer *r = checked_malloc(sizeof(struct sprite_renderer));
    r->projection = projection;

    return r;
}

