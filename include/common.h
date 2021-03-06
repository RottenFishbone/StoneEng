#pragma once 
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include "zf_log.h"
#define ZF_LOG_LEVEL ZF_LOG_INFO
// gfx
#include "glad/gl.h"
#include <GLFW/glfw3.h>

#define DEBUG_WARN

#define RESOLUTION_X 800.f    
#define RESOLUTION_Y 600.f
#define FULLSCREEN 0

#define CHUNK_WIDTH 16
#define CHUNK_AREA (CHUNK_WIDTH * CHUNK_WIDTH)

// Macros
#define Diag_IgnoreUnused \
    _Pragma("GCC diagnostic push") \
    _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"") 
#define Diag_Pop \
    _Pragma("GCC diagnostic pop") 

#define TGL_FLAG(n, f) ((n) ^= (f))
#define SET_FLAG(n, f) ((n) |= (f))
#define CLR_FLAG(n, f) ((n) &= ~(f))
#define CHK_FLAG(n, f) ((n) & (f))

#define min(a,b) __extension__ \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
       _a < _b ? _a : _b; })

#define max(a,b) __extension__ \
    ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
       _a > _b ? _a : _b; })

void *checked_malloc(size_t n);
void todo();

// Vector Types
// --------------
// Short vectors (Maybe remove?)
#define short_vec2(a,b) ((short_vec2){.x=a,.y=b})
typedef struct {
    int16_t x;
    int16_t y;
} short_vec2;
#define ushort_vec2(a,b) ((ushort_vec2){.x=a,.y=b})
typedef struct {
    uint16_t x;
    uint16_t y;
} ushort_vec2;

// Integer vectors
#define vec2(a,b) ((vec2){.x=a,.y=b})
typedef struct {
    int32_t x;
    int32_t y;
} vec2;
#define vec3(a,b,c) ((vec3){.x=a,.y=b,.z=c})
typedef struct {
    int32_t x;
    int32_t y;
    int32_t z;
} vec3;
#define vec4(a,b,c,d) ((vec4){.x=a,.y=b,.z=c,.w=d})
typedef struct {
    int32_t x;
    int32_t y;
    int32_t z;
    int32_t w;
} vec4;

// Float vectors
#define fvec2(a,b) ((fvec2){.x=a,.y=b})
typedef struct {
    float x;
    float y;
} fvec2;

#define fvec3(a,b,c) ((fvec3){.x=a,.y=b,.z=c})//
typedef struct {
    float x;
    float y;
    float z;
} fvec3;
#define fvec4(a,b,c,d) ((fvec4){.x=a,.y=b,.z=c,.w=d})//
typedef struct {
    float x;
    float y;
    float z;
    float w;
} fvec4;
// ----------------


// Linear algebra
// ---------------
typedef struct {
    float m[16];
} mat4;

static const mat4 IDENTITY_MATRIX = {
    .m = {
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0,
    },
};

mat4 lalg_cross(mat4 *m1, mat4 *m2);
mat4 lalg_ortho(float left, float right, float bottom, float top);
void lalg_ortho_into(float left, float right, float bottom, float top, mat4 *);
mat4 lalg_translation(float x, float y, float z);
void lalg_print(mat4*);
// -------------
