#include "../../include/gfx/shaders.h"

int shader_program_from_files(GLuint *program, const char *vert_path,
                              const char *frag_path, const char *geom_path) {
    
    GLuint vert_shader, frag_shader, geom_shader;
    geom_shader = 0;
    
    // Compile vertex and fragment shaders (required)
    if (!shader_compile_file(&vert_shader, GL_VERTEX_SHADER, vert_path)   ||
        !shader_compile_file(&frag_shader, GL_FRAGMENT_SHADER, frag_path)) {
        goto err;
    }
    // Compile geometry shader, if provided
    if (geom_path != NULL && 
        !shader_compile_file(&geom_shader, GL_GEOMETRY_SHADER, geom_path)){
        goto err;
    }

    // Link all the compiled shaders into a program
    if (!shader_program_link(program, vert_shader, frag_shader, geom_shader)){
        goto err;
    }
    
    // Cleanup shader objects
    glDeleteShader(vert_shader);
    glDeleteShader(frag_shader);
    if (geom_shader) { glDeleteShader(geom_shader); }

    return 1;

err:
    ZF_LOGE("Failed to build shader program.\n");
    return 0;
}

int shader_program_link(GLuint *program, GLuint vertex_shader, 
                        GLuint fragment_shader, GLuint geometry_shader) {

    *program = glCreateProgram();
    glAttachShader(*program, vertex_shader);
    glAttachShader(*program, fragment_shader);
    if (geometry_shader) { glAttachShader(*program, geometry_shader); }
    glLinkProgram(*program);

    GLint link_success;
    GLchar link_log[512];
    glGetProgramiv(*program, GL_LINK_STATUS, &link_success);
    if (!link_success){
        glGetProgramInfoLog(*program, 512, NULL, link_log);
        ZF_LOGE("Failed to link shader program:\n%s\n", link_log);
        goto err;
    }

    glDetachShader(*program, vertex_shader);
    glDetachShader(*program, fragment_shader);
    if (geometry_shader) { glDetachShader(*program, geometry_shader); }
    return 1;
err:
    return 0;
}

int shader_compile_file(GLuint *shader, GLenum shader_type, const char *path){
    // Open shader file
    FILE *file = fopen(path, "r");
    if (!file){
        ZF_LOGE("Failed to open shader file \"%s\"\n", path);
        goto err;
    }

    // Get file size by scanning to the end then resetting
    fseek(file, 0, SEEK_END);
    size_t num_bytes = ftell(file);
    if (!num_bytes){
        ZF_LOGE("Shader file \"%s\" is empty.\n", path);
        goto err_after_file;
    }
    fseek(file, 0, SEEK_SET);
    
    // Read file into buffer
    char *buffer = calloc(num_bytes+1, sizeof(char));
    if (!buffer){
        ZF_LOGF("Failed to allocate buffer while loading shader file\n");
        goto err_after_file;
    }
    fread(buffer, sizeof(char), num_bytes, file);

    // Compile shader from buffer
    *shader = glCreateShader(shader_type);
    const char *src = (const char *) buffer;
    glShaderSource(*shader, 1, &src, NULL);
    glCompileShader(*shader);
        
    // Memory cleanup
    fclose(file);
    free(buffer);
    
    // Check for compilation success
    GLint compile_success;
    GLchar compile_log[512];
    glGetShaderiv(*shader, GL_COMPILE_STATUS, &compile_success);
    if (!compile_success) {
        glGetShaderInfoLog(*shader, 512, NULL, compile_log);
        ZF_LOGE("Failed to compile shader \"%s\":\n%s\n",
                path, compile_log);
        goto err;
    }
    
    return 1;

err_after_file:
    fclose(file);
err:
    return 0;
}
