#pragma once
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "../common.h"

#define NUM_HOOKS 4
enum input_hook {
    hook_mouse_btn,
    hook_mouse_pos,
    hook_scroll,
    hook_key,
};

struct input_globals {
    fvec2 mouse_pos;
    int mouse_btns[3];
    int mouse_modifier;

    int key[512];
    int key_scancode;
    int key_modifier;
    
    double scroll_xoffset;
    double scroll_yoffset;
};

struct input_callbacks {
    void (*key) (GLFWwindow*, int, int, int, int);
    void (*scroll) (GLFWwindow*, double, double);
    void (*mouse_btn) (GLFWwindow*, int, int, int);
    void (*mouse_pos) (GLFWwindow*, double, double);
};

/// Return the list of callback pointers, these are intended to initialize
/// GLFW.
const struct input_callbacks *input_get_callbacks();

/// Returns a pointer to a struct containing input data
struct input_globals *input_get_globals();

/// Hooks a function to be called on a GLFW callback. The type of callback to 
/// hook into is designated using hook_type.
/// This allows multiple input callbacks to be used throughout code interacting
/// with the engine. 
void input_add_hook(void (*fn)(), enum input_hook);

// TODO remove_hook, set_hook_order, hook_priority_level?
