#include "../include/stone_eng.h"

static fvec2 camera = (fvec2) { 0.0, 0.0 };

Diag_IgnoreUnused
void on_key(int key, int scancode, int action, int mods){
    const int STEP = 32;

    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_DOWN: 
                camera.y += STEP;
                break;
            case GLFW_KEY_UP: 
                camera.y -= STEP;
                break;
            case GLFW_KEY_LEFT:
                camera.x += STEP;
                break;
            case GLFW_KEY_RIGHT:
                camera.x -= STEP;
                break;
            default:
                break;
        }
    }
}

void on_scroll(double xoff, double yoff){
    // TODO zooming
}
Diag_Pop

int main() {
    GLFWwindow *window;
    gfx_init(&window);
  
    mat4 projection = lalg_ortho(0, RESOLUTION_X, 0, RESOLUTION_Y);

    input_add_hook(&on_key, hook_key);
    input_add_hook(&on_scroll, hook_scroll);

    // Render order:
    // terrain -> bitmasked tiles (transitions and overlays) -> entities 
    // particles -> UI
    
    // The terrain renderer renders base tiles, such as dirt and water, these
    // are later covered by masking tiles such as corner tiles
    struct tile_renderer *terrain_renderer = tile_renderer_init(&camera, &projection);
    struct text_renderer *text_renderer = text_renderer_init(&projection);
    struct text_block *fps_block = text_renderer_new_block(text_renderer);
    fps_block->anchor_pos = (fvec2) { .x = 10.f, .y = (float) RESOLUTION_Y - 10.f };
    fps_block->font_size = 1.f;

    text_renderer_update(text_renderer);


    for (size_t i = 0; i < CHUNK_WIDTH; ++i) {
        tile_renderer_set_tile(terrain_renderer, 2, (vec2){.x=i, .y=0});
    }

    // Main execution loop
    unsigned frames_this_sec = 0;
    double time_since_report = glfwGetTime();
    glfwSwapInterval(1);
    while (!glfwWindowShouldClose(window)){
        // FPS reporting/tracking
        ++frames_this_sec;
        if (glfwGetTime() - time_since_report >= 1.0) {
            char fps_chars[128];
            if (frames_this_sec < 10000u){
                sprintf(fps_chars, "FPS: %u\nFT: %.2fms", 
                        frames_this_sec, 1.0 / (float) (frames_this_sec) * 1000.f);
            } else {
                sprintf(fps_chars, "FPS: Bruh."); 
            }

            text_block_edit_contents(fps_block, fps_chars);
            text_renderer_update(text_renderer);
            frames_this_sec = 0;
            time_since_report = glfwGetTime();
        }
        
        glClear(GL_COLOR_BUFFER_BIT);
        tile_renderer_draw(terrain_renderer);
        text_renderer_draw(text_renderer);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    free_text_renderer(text_renderer);
    free_tile_renderer(terrain_renderer);
    return 0;
}
