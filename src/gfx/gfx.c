#include "../../include/gfx/gfx.h"
#include <GLFW/glfw3.h>
// stb_image is implemented here to allow for additional includes by the renderers
#define STB_IMAGE_IMPLEMENTATION
#include "../../include/stb_image.h"

int gfx_init(GLFWwindow **window, const char *title) {
    glfwInit();

    // OpenGL window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create window using default parameters
    *window = glfwCreateWindow(RESOLUTION_X, RESOLUTION_Y, 
                                title, 
                                FULLSCREEN ? glfwGetPrimaryMonitor() : NULL, 
                                NULL);
    if (!window){
        ZF_LOGF("[GLFW3] Failed to create window.");
        goto err;
    }
    glfwMakeContextCurrent(*window);

    // Load OpenGL functions
    if (!gladLoadGL(glfwGetProcAddress)){
        ZF_LOGF("[GLAD] Failed to load OpenGL function pointers.");
        goto err;
    }
    
    // Set OpenGL vars
    glEnable(GL_BLEND); // Enable transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0);
    glDepthFunc(GL_LEQUAL);

    glClearColor(0.2, 0.2, 0.2, 1.0);            // Clear to grey
    glViewport(0,0, RESOLUTION_X, RESOLUTION_Y); // Init viewport scale 

    // Set callbacks
    const struct input_callbacks *callbacks = input_get_callbacks();
    glfwSetKeyCallback(*window, callbacks->key); 
    glfwSetScrollCallback(*window, callbacks->scroll); 
    glfwSetMouseButtonCallback(*window, callbacks->mouse_btn);
    glfwSetCursorPosCallback(*window, callbacks->mouse_pos);
    
    // Load renderers
    // TODO

    ZF_LOGI("OpenGL/GLFW successfully initialized."); 
    return 1;
err:
    glfwTerminate();
    return 0;
}

Diag_IgnoreUnused
void gfx_load_callbacks(GLFWwindow **window){
    //TODO implement
    todo();
}
Diag_Pop

void gfx_terminate(GLFWwindow **window) {
    // free callbacks
    glfwSetKeyCallback(*window, NULL);
    glfwSetCursorPosCallback(*window, NULL);
    glfwSetMouseButtonCallback(*window, NULL);

    // free the window
    glfwDestroyWindow(*window);

    glfwTerminate();
}
