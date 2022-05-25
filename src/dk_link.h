#pragma once

#include <vx_utils.h>

typedef struct {
    i32 from;
    i32 to;

    i32 cost;

    bool bidirectional;
    i32 cost_reverse;
} dk_Link;
VX_CREATE_DEFAULT(dk_Link,
    .from = 0,
    .to = 0,
    .cost_reverse = 0,
    .cost = 0,
    .bidirectional = false
)

_VX_OPTION_CREATE_FOR_TYPE(dk_Link)
_VX_VECTOR_CREATE_FOR_TYPE(dk_Link)