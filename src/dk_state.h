#pragma once
#include <vx_utils.h>

#include "dk_matrix.h"
#include "dk_router.h"
#include "dk_link.h"

typedef enum {
    DK_ADD_ROUTER,
    DK_ADD_LINK,
    DK_REMOVE,
    DK_MODIFY,
    DK_STATEINPUTMODE_COUNT
} dk_StateInputMode;

typedef struct {
    VX_T(dk_Router, vx_Vector) routers;
    VX_T(dk_Link, vx_Vector) links;

    i32 start;
    i32 end;

    dk_Matrix map;
    VX_T(i32, vx_Vector) optimal_links;
    i32 optimal_cost;

    dk_StateInputMode input_mode;

    i32 last_selected;
} dk_State;

extern dk_State DK_STATE_INSTANCE;

void dk_state_init();
void dk_state_free();

void dk_state_logic();
void dk_state_draw();

void dk_state_change_inputmode(dk_StateInputMode mode);
void dk_stateinputmode_to_string(dk_StateInputMode mode, char* buffer, int size, int start_idx);