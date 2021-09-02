#pragma once
#include "../../common.h"
#include "../shaders.h"

#define SHADER_TEXT_VERT "assets/shaders/text_vert.glsl"
#define SHADER_TEXT_FRAG "assets/shaders/text_frag.glsl"
#define SHADER_TEXT_GEOM "assets/shaders/text_geom.glsl"
#define FONT_IMG_PATH "assets/textures/fonts/dogica-bitmap-font-inverted.png"

/// LIMITATION:
// Currently text_renderer's shaders and renderer code can only handle a 
// 8x8 font with 16 chars per row. The font is in ascii order and must start
// at '!'.
// TODO This could be expanded to be more robust while offering this as default
// behaviour.

/// FOREWARNING:
// Updating rendered text causes a complete rebuild of the rendered data.
// This is obviously very inefficient for large amounts of text but likely 
// inconsequential for a UI.
// TODO Provide a block of allocated memory for each text block on the GPU
// and update in chunks vs entirety.

// TODO Font size (scaling) and color.

// TODO Add some sort of testing state?

/// A block of text to be rendered by a text_renderer
struct text_block {
    char *contents;
    size_t length;
    
    float font_size;
    fvec3 font_color;
    fvec2 anchor_pos;
};


/// Initialize a new instance of a text_renderer. This is geared at creating UI
/// and Debug text as it is rendered in screen space with no native ability to 
/// have a camera matrix. 
/// Text is passed through text_blocks using text_renderer_push_block and is 
/// only updated on a text_renderer_update call.
/// The font is rendered using a monospace bitmap font and only supports ASCII
/// characters (Not extended, although that is easily made achievable). 
struct text_renderer *text_renderer_init(mat4 *projection);

/// Creates an empty text_block on the target text_renderer and returns a
/// pointer to that text_block.
struct text_block *text_renderer_new_block(struct text_renderer*);

void text_block_edit_contents(struct text_block*, char*);
void text_renderer_remove_block(struct text_renderer*, struct text_block*);

/// Parse all the stored text_blocks into renderable data and send to the GPU for
/// storage. As such, this renderer is not suited for LARGE amounts of text or 
/// large amounts of dynamic text.
void text_renderer_update(struct text_renderer*);

void text_renderer_draw(struct text_renderer*);
void free_text_renderer(struct text_renderer*);
