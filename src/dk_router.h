#pragma once

#include <raylib.h>
#include <vx_utils.h>

static const f32 DK_ROUTER_RADIUS = 20.0f;

typedef struct {
    char* identifier;
    Color color;
    Vector2 position;
} dk_Router;
VX_CREATE_DEFAULT(dk_Router,
    .identifier = NULL,
    .color = BLACK,
    .position = (Vector2){ 0, 0 }
)

dk_Router dk_router_from_identifier(char* identifier);
void dk_router_free(dk_Router* router);

_VX_OPTION_CREATE_FOR_TYPE(dk_Router)
_VX_VECTOR_CREATE_FOR_TYPE(dk_Router)