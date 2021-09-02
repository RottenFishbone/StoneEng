#include "../../include/gfx/input.h"
#include <GLFW/glfw3.h>

// A container for the various hooks
struct input_hooks {
    void (**hooks[NUM_HOOKS])();
    size_t num_hooks[NUM_HOOKS];
};

// Static instances of hooks and global input values.
// These are modified when new hooks are added via the 'public' input_add_hook
static struct input_globals globals;
static struct input_hooks input_hooks; 

Diag_IgnoreUnused
// To be called by GLFW during initialization. To access the callback elsewhere
// input_add_hook can be used to hook a function pointer into this callback.
void input_key_callback(GLFWwindow *window, int key, 
                        int scancode, int action, int mods){
    
    // Intercept important keys to be handled by default
    if (action == GLFW_PRESS){
        if (key == GLFW_KEY_ESCAPE || 
           (key == GLFW_KEY_F4 && mods & GLFW_MOD_ALT)){
            glfwSetWindowShouldClose(window, 1);
            return;
        }
    }
    
    // Set the globals to reflect key press
    globals.key_scancode = scancode;
    globals.key_modifier = mods;
    globals.key[key] = action;

    // Call all hooked functions
    for (size_t i = 0; i < input_hooks.num_hooks[hook_key]; i++){
        input_hooks.hooks[hook_key][i](key, scancode, action, mods);
    }
}

// See input_key_callback for detailed comments
void input_scroll_callback(GLFWwindow *window, double xoffset, double yoffset){
    globals.scroll_xoffset = xoffset;
    globals.scroll_yoffset = yoffset;

    for (size_t i = 0; i < input_hooks.num_hooks[hook_scroll]; ++i) {
        input_hooks.hooks[hook_scroll][i](xoffset, yoffset);
    }
}

// See input_key_callback for detailed comments
void input_mouse_btn_callback(GLFWwindow *window, int button, int action, int mods){
    globals.mouse_btns[button] = action;
    globals.mouse_modifier = mods;

    for (size_t i = 0; i < input_hooks.num_hooks[hook_mouse_btn]; ++i){
        input_hooks.hooks[hook_mouse_btn][i](button, action, mods);
    }
}

void input_mouse_pos_callback(GLFWwindow *window, double xpos, double ypos){
    globals.mouse_pos = (fvec2) { .x=xpos, .y=ypos };

    for (size_t i = 0; i < input_hooks.num_hooks[hook_mouse_pos]; ++i){
        input_hooks.hooks[hook_mouse_pos][i](xpos, ypos);        
    }
}

// Assign the callback functions to be used by GLFW during gfx init
static const struct input_callbacks callbacks = {
    .key = &input_key_callback,
    .scroll = &input_scroll_callback,
};

// Static pointer accessor
struct input_globals *input_get_globals(){
    return &globals;
}
// Static pointer accessor
const struct input_callbacks *input_get_callbacks(){
    return &callbacks;
}
Diag_Pop

void input_add_hook(void (*fn)(), enum input_hook hook_type){
    if (hook_type >= NUM_HOOKS) { 
        ZF_LOGE("Invalid hook_type passed (%d), add_hook ignored.\n", hook_type);
    }
    
    // Grab the relevant hook type's storage arrays
    size_t *count = &(input_hooks.num_hooks[hook_type]);
    void (***hooks)() = &(input_hooks.hooks[hook_type]);

    // Create a larger hook array
    void (**new_hooks)() = calloc((*count)+1, sizeof(fn));
    if (count){
        // Clone the old hooks into the new one, if possible
        memcpy(new_hooks, *hooks, sizeof(fn)* (*count));
        free(*hooks);
    }
    // Move the hooks pointer to the new array
    *hooks = new_hooks;
    // Add the newest hook to the array
    *hooks[*count] = fn;
    *count += 1;
}
