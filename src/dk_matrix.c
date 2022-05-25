#include "dk_matrix.h"

_VX_OPTION_CREATE_FOR_TYPE(VX_T(i32, vx_Vector))
_VX_VECTOR_CREATE_FOR_TYPE(VX_T(i32, vx_Vector))

dk_Matrix dk_matrix_new(u32 size) {
    VX_ASSERT("Size must be > 0", size > 0)

    dk_Matrix matrix;

    matrix.data = vx_smalloc(sizeof(i32) * size * size);
    matrix.size = size;

    memset(matrix.data, 0, sizeof(i32) * size * size);

    return matrix;
}

dk_Matrix dk_matrix_from_data(u32 size, i32* data) {
    VX_ASSERT("Size must be >= 0", size >= 0)

    dk_Matrix matrix;

    matrix.data = data;
    matrix.size = size;

    return matrix;
}

void dk_matrix_free(dk_Matrix* matrix) {
    vx_free(matrix->data);
}

i32 dk_dijkstra_distance(dk_Matrix* matrix, u32 start, u32 end) {
    // the current distance from the start node and the end node.
    i32* distance = vx_smalloc(sizeof(i32) * matrix->size);

    distance[start] = 0;
    for (int i = 1; i < matrix->size; i++) {
        int current_node_idx = (i + start) % matrix->size;

        distance[current_node_idx] = DK_INFINITY;
    }

    for (int i = 0; i < matrix->size; i++) {
        int current_node_idx = (i + start) % matrix->size;

        // iterate over all the other nodes.
        for (int j = 0; j < matrix->size; j++) {
            if (DK_MATRIX_DATA(matrix, current_node_idx, j) == DK_NO_LINK ||
                distance[current_node_idx] + DK_MATRIX_DATA(matrix, current_node_idx, j) >= distance[j]
            ) {
                continue;
            }

            distance[j] = distance[current_node_idx] + DK_MATRIX_DATA(matrix, current_node_idx, j);
        }
    }

    int res = distance[end];

    vx_free(distance);

    return res;
}

i32 dk_dijkstra_path(dk_Matrix* matrix, u32 start, u32 end, VX_T(i32, vx_Vector)* buffer) {
    // the current distance from the start node and the end node.
    i32* distance = vx_smalloc(sizeof(i32) * matrix->size);
    bool* visited = vx_smalloc(sizeof(bool) * matrix->size);
    bool* reached = vx_smalloc(sizeof(bool) * matrix->size);

    memset(visited, 0, sizeof(bool) * matrix->size);
    memset(reached, 0, sizeof(bool) * matrix->size);
    visited[start] = true;
    reached[start] = true;
    int num_visited = 1;
    int num_reached = 1;

    VX_T(VX_T(i32, vx_Vector), vx_Vector) paths = VX_T(VX_T(i32, vx_Vector), vx_vector_new)();
    for (int i = 0; i < matrix->size; i++) {
        VX_T(i32, vx_Vector) vec = VX_T(i32, vx_vector_new)();
        VX_T(VX_T(i32, vx_Vector), vx_vector_push)(&paths, vec);
    }

    distance[start] = 0;
    for (int i = 1; i < matrix->size; i++) {
        int current_node_idx = (i + start) % matrix->size;

        distance[current_node_idx] = DK_INFINITY;
    }

    for (int i = 0; num_visited < matrix->size && num_reached < matrix->size; i++) {
        int current_node_idx = (i + start) % matrix->size;

        if (!reached[current_node_idx]) {
            continue;
        } else {
            if (!visited[current_node_idx]) {
                visited[current_node_idx] = true;
                num_visited++;
            }
        }

        // iterate over all the other nodes.
        for (int j = 0; j < matrix->size; j++) {
            printf("from %d to %d: cost %d vs %d ", current_node_idx, j, distance[current_node_idx] + DK_MATRIX_DATA(matrix, current_node_idx, j), distance[j]);

            if (DK_MATRIX_DATA(matrix, current_node_idx, j) == DK_NO_LINK ||
                distance[current_node_idx] + DK_MATRIX_DATA(matrix, current_node_idx, j) >= distance[j]
            ) {
                printf("NO\n");
                continue;
            }

            printf("YES\n");

            distance[j] = distance[current_node_idx] + DK_MATRIX_DATA(matrix, current_node_idx, j);
            VX_T(i32, vx_vector_clone)(&(VX_VD(&paths)[current_node_idx]), &(VX_VD(&paths)[j]));
            VX_T(i32, vx_vector_push)(&(VX_VD(&paths)[j]), j);
            if (reached[j] != true) {
                reached[j] = true;
                num_reached++;
            }
        }

        printf("%d\n", num_visited);
    }

    int res = distance[end];

    VX_T(i32, vx_vector_clone)(&(VX_VD(&paths)[end]), buffer);

    vx_free(reached);
    vx_free(distance);
    vx_free(visited);
    for (int i = 0; i < matrix->size; i++) {
        VX_T(i32, vx_vector_free)(&(VX_VD(&paths)[i]));
    }
    VX_T(VX_T(i32, vx_Vector), vx_vector_free)(&paths);

    return res;
}