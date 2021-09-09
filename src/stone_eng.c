#include "../include/stone_eng.h"

#define ATLAS_PATH "assets/textures/atlas.png"
#define ATLAS_TILE_SIZE 32

static fvec2 camera = fvec2(0.f,0.f);
static fvec2 camera_offset = fvec2(0.f, 0.f);
static mat4 *cam_projection = NULL;
static float zoom_level = 1;

Diag_IgnoreUnused
void on_key(int key, int scancode, int action, int mods){
    const float STEP = 32.f;

    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_DOWN: 
                camera_offset.y += STEP*zoom_level;
                break;
            case GLFW_KEY_UP: 
                camera_offset.y -= STEP*zoom_level;
                break;
            case GLFW_KEY_LEFT:
                camera_offset.x += STEP*zoom_level;
                break;
            case GLFW_KEY_RIGHT:
                camera_offset.x -= STEP*zoom_level;
                break;
            default:
                break;
        }
    }
    camera = fvec2(camera_offset.x, camera_offset.y);
}

void on_scroll(double xoff, double yoff){
    if (yoff > 0){
        zoom_level *= 2.f;
    } else {
        zoom_level /= 2.f;
    }
    lalg_ortho_into(0, RESOLUTION_X/zoom_level, 0, RESOLUTION_Y/zoom_level, cam_projection);
    //camera = fvec2(RESOLUTION_X/2.0/zoom_level, RESOLUTION_Y/2.0/zoom_level);
}
Diag_Pop

int main() {
    // Initialize window
    GLFWwindow *window;
    gfx_init(&window, "StoneEng -- TEST");
    

    // Build projection martricies
    mat4 scr_projection = lalg_ortho(0, RESOLUTION_X, 0, RESOLUTION_Y); 
    cam_projection = checked_malloc(sizeof(mat4));
    lalg_ortho_into(0, RESOLUTION_X, 0, RESOLUTION_Y, cam_projection);

    // Add input hooks
    input_add_hook(&on_key, hook_key);
    input_add_hook(&on_scroll, hook_scroll);

    // Render order:
    // terrain -> bitmasked tiles (transitions and overlays) -> entities 
    // particles -> UI
    
    // The terrain renderer renders base tiles, such as dirt and water, these
    // are later covered by masking tiles such as corner tiles
    struct tile_renderer *terrain_renderer = tile_renderer_init(&camera, cam_projection, 
                                                                ATLAS_PATH, ATLAS_TILE_SIZE);
    struct tile_renderer *tile_renderer = tile_renderer_init(&camera, cam_projection, 
                                                             ATLAS_PATH, ATLAS_TILE_SIZE);

    struct text_renderer *text_renderer = text_renderer_init(&scr_projection);
    struct text_block *fps_block = text_renderer_new_block(text_renderer);
    fps_block->anchor_pos = (fvec2) { .x = 10.f, .y = (float) RESOLUTION_Y - 10.f };
    fps_block->font_size = 1.f;

    text_renderer_update(text_renderer);


    for (size_t i = 0; i < CHUNK_AREA; ++i) {
        tile_renderer_set_index(terrain_renderer, 1, i);
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
            text_renderer_draw(text_renderer);
            frames_this_sec = 0;
            time_since_report = glfwGetTime();
        }
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        tile_renderer_set_index(tile_renderer, rand() % 3, rand() % CHUNK_AREA);
        tile_renderer_draw(terrain_renderer);
        tile_renderer_draw(tile_renderer);
        text_renderer_draw(text_renderer);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    free(cam_projection);

    free_text_renderer(text_renderer);
    free_tile_renderer(terrain_renderer);
    free_tile_renderer(tile_renderer);

    gfx_terminate(&window);
    return 0;
}
