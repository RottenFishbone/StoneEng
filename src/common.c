#include "../include/common.h"

void todo() {
    ZF_LOGE("Called unimplemented function.\n");
    exit(EXIT_FAILURE);
}

void *checked_malloc(size_t n){
    void *mem = malloc(n);
    if (!mem){
        ZF_LOGF("Failed to allocate memory.\n");
        exit(EXIT_FAILURE);
    }

    return mem;
}


mat4 lalg_cross(mat4 *m1, mat4 *m2){
    mat4 m;
    memcpy(&m, m1, sizeof(mat4));

    for (int r = 0; r < 4; ++r){
        int row_id = r * 4;
        for (int c = 0; c < 4; ++c){
            m.m[row_id + c] = 
                (m1->m[row_id + 0] * m2->m[c + 0x0]) +
                (m1->m[row_id + 1] * m2->m[c + 0x4]) +
                (m1->m[row_id + 2] * m2->m[c + 0x8]) + 
                (m1->m[row_id + 3] * m2->m[c + 0xC]);
        }
    }
    return m;
}

mat4 lalg_ortho(float left, float right, float bottom, float top){
    mat4 matrix;

    memset(matrix.m, 0, sizeof(matrix.m));

    matrix.m[0x0] = 2.0/(right-left);
    matrix.m[0x5] = 2.0/(top-bottom);
    matrix.m[0xA] =  1.0;

    matrix.m[0xC] = -1.0;
    matrix.m[0xD] = -1.0;
    matrix.m[0xF] =  1.0;

    // 0 1 2 3
    // 4 5 6 7
    // 8 9 A B
    // C D E F

    return matrix;
}
void lalg_ortho_into(float left, float right, float bottom, float top, mat4 *matrix){
    memset(matrix->m, 0, sizeof(matrix->m));

    matrix->m[0x0] = 2.0/(right-left);
    matrix->m[0x5] = 2.0/(top-bottom);
    matrix->m[0xA] =  1.0;

    matrix->m[0xC] = -1.0;
    matrix->m[0xD] = -1.0;
    matrix->m[0xF] =  1.0;
}

mat4 lalg_translation(float x, float y, float z){
    mat4 matrix = IDENTITY_MATRIX;
    matrix.m[0xC] = x;
    matrix.m[0xD] = y;
    matrix.m[0xE] = z;

    return matrix;
}

void lalg_print(mat4 *m){
    printf("-----MAT4-----\n");
    for (int i = 0; i < 16; ++i) {
        printf("%f ", m->m[i]);

        if ((i+1) % 4 == 0){
            printf("\n");
        }
    }
    printf("-------------\n");
}

