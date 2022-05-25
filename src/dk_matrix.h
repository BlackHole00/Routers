#pragma once

#include <vx_utils.h>

#define DK_INFINITY INT16_MAX
#define DK_NO_LINK DK_INFINITY
#define DK_MATRIX_DATA(_MATRIX, _X, _Y) (_MATRIX)->data[(_Y) + ((_X) * (_MATRIX)->size)]

typedef struct {
    i32* data;
    u32 size;
} dk_Matrix;

dk_Matrix dk_matrix_new(u32 size);
dk_Matrix dk_matrix_from_data(u32 size, i32* data);
void dk_matrix_free(dk_Matrix* matrix);

i32 dk_dijkstra_distance(dk_Matrix* matrix, u32 start, u32 end);
i32 dk_dijkstra_path(dk_Matrix* matrix, u32 start, u32 end, VX_T(i32, vx_Vector)* buffer);