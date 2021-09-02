#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "../common.h"

/// shader_program_from_files
///
/// Generate an OpenGL program using glsl filepaths.
/// Note, geom_path can be left NULL.
int shader_program_from_files(GLuint *program, 
                              const char *vert_path, 
                              const char *frag_path,
                              const char *geom_path);

/// shader_program_link
///
/// Links a set of compiled shaders, and cleans up the shader objects.
/// Used as a helper function for "shader_program_from_files()", though can
/// be called explicitly.
/// Note, geometry_shader of 0 will link without one.
int shader_program_link(GLuint *program, 
                        GLuint vertex_shader, 
                        GLuint fragment_shader,
                        GLuint geometry_shader);

/// shader_compile_file
///
/// Compiles an individual shader file from a given filepath.
int shader_compile_file(GLuint *shader, 
                        GLenum shader_type, 
                        const char *path);
