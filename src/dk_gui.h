#pragma once

#include <vx_utils.h>
#include <raylib-nuklear.h>

static const i32 FONT_SIZE = 10;

typedef struct {
    struct nk_context *nk_context;

    struct {
        char router_name[50];
        Color router_color;
    } add_router_mode;

    struct {
        char cost_str[20];
        char reverse_cost_str[20];

        i32 cost;
        bool bidirectional;
        i32 reverse_cost;
    } add_link_mode;

    struct {
        i32 current_modify;
        bool modify_is_line;

        char cost_str[20];
        char reverse_cost_str[20];

        i32 cost;
        bool bidirectional;
        i32 reverse_cost;

        char router_name[50];
        Color router_color;

        bool request;
        bool ready;
    } modify_mode;

    struct {
        bool select_start_required;
        bool select_end_required;

        bool run;
        i32 start;
        i32 end;
    } calculate;
} dk_Gui;

extern dk_Gui DK_GUI_INSTANCE;

void dk_gui_init();
void dk_gui_logic();
void dk_gui_draw();