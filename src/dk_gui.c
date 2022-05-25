#include "dk_gui.h"

#include <string.h>
#include <stdlib.h>
#include "dk_state.h"

dk_Gui DK_GUI_INSTANCE;

static void _dk_gui_calculate() {
    int window_width = GetScreenWidth();

    if (nk_begin(DK_GUI_INSTANCE.nk_context, "Calculate", nk_rect(window_width - 440, 0, 220, 175),
        NK_WINDOW_BORDER|NK_WINDOW_MINIMIZABLE)
    ) {
        static char from[50];
        if (DK_GUI_INSTANCE.calculate.start != -1) {
            snprintf(from, 50, "From: %s", VX_VD(&DK_STATE_INSTANCE.routers)[DK_GUI_INSTANCE.calculate.start].identifier);
        } else {
            strcpy(from, "From: ");
        }

        static char to[50];
        if (DK_GUI_INSTANCE.calculate.end != -1) {
            snprintf(to, 50, "To: %s", VX_VD(&DK_STATE_INSTANCE.routers)[DK_GUI_INSTANCE.calculate.end].identifier);
        } else {
            strcpy(to, "To: ");
        }

        nk_layout_row_dynamic(DK_GUI_INSTANCE.nk_context, 10, 1);
        nk_label(DK_GUI_INSTANCE.nk_context, from, NK_TEXT_ALIGN_LEFT | NK_TEXT_ALIGN_BOTTOM);
        nk_layout_row_dynamic(DK_GUI_INSTANCE.nk_context, 15, 1);
        if (nk_button_label(DK_GUI_INSTANCE.nk_context, DK_GUI_INSTANCE.calculate.select_start_required ? "Selecting..." : "SELECT")) {
            DK_GUI_INSTANCE.calculate.select_start_required = true;
            DK_GUI_INSTANCE.calculate.select_end_required = false;

            VX_T(i32, vx_vector_clear)(&DK_STATE_INSTANCE.optimal_links);
            DK_STATE_INSTANCE.optimal_cost = -1;
        }

        nk_layout_row_dynamic(DK_GUI_INSTANCE.nk_context, 15, 1);
        nk_label(DK_GUI_INSTANCE.nk_context, to, NK_TEXT_ALIGN_LEFT | NK_TEXT_ALIGN_BOTTOM);
        nk_layout_row_dynamic(DK_GUI_INSTANCE.nk_context, 15, 1);
        if (nk_button_label(DK_GUI_INSTANCE.nk_context, DK_GUI_INSTANCE.calculate.select_end_required ? "Selecting..." : "SELECT")) {
            DK_GUI_INSTANCE.calculate.select_end_required = true;
            DK_GUI_INSTANCE.calculate.select_start_required = false;

            VX_T(i32, vx_vector_clear)(&DK_STATE_INSTANCE.optimal_links);
            DK_STATE_INSTANCE.optimal_cost = -1;
        }

        nk_layout_row_dynamic(DK_GUI_INSTANCE.nk_context, 10, 1);
        nk_spacer(DK_GUI_INSTANCE.nk_context);

        nk_layout_row_dynamic(DK_GUI_INSTANCE.nk_context, 27, 1);
        if (nk_button_label(DK_GUI_INSTANCE.nk_context, "CALCULATE")) {
            DK_GUI_INSTANCE.calculate.run = true;
        }

        if (DK_STATE_INSTANCE.optimal_cost != -1) {
            char result[64];
            if (DK_STATE_INSTANCE.optimal_cost != DK_INFINITY) {
                snprintf(result, 64, "Cost: %d", DK_STATE_INSTANCE.optimal_cost);
            } else {
                snprintf(result, 64, "No Link!");
            }

            nk_layout_row_dynamic(DK_GUI_INSTANCE.nk_context, 15, 1);
            nk_label(DK_GUI_INSTANCE.nk_context, result, NK_TEXT_ALIGN_LEFT | NK_TEXT_ALIGN_BOTTOM);
        }
    }
    nk_end(DK_GUI_INSTANCE.nk_context);
}

static void _dk_gui_common() {
    int window_width = GetScreenWidth();

    if (nk_begin(DK_GUI_INSTANCE.nk_context, "Commands", nk_rect(window_width - 220, 0, 220, 175),
        NK_WINDOW_BORDER|NK_WINDOW_MINIMIZABLE)
    ) {
        static char mode[50];
        nk_layout_row_dynamic(DK_GUI_INSTANCE.nk_context, 30, 1);
        strcpy(mode, "CURRENT MODE: ");
        dk_stateinputmode_to_string(DK_STATE_INSTANCE.input_mode, mode, 50, 14);
        nk_label(DK_GUI_INSTANCE.nk_context, mode, NK_TEXT_CENTERED);

        nk_layout_row_dynamic(DK_GUI_INSTANCE.nk_context, 20, 1);
        if (nk_button_label(DK_GUI_INSTANCE.nk_context, "Router")) {
            dk_state_change_inputmode(DK_ADD_ROUTER);
        }
        if (nk_button_label(DK_GUI_INSTANCE.nk_context, "Link")) {
            dk_state_change_inputmode(DK_ADD_LINK);
        }
        if (nk_button_label(DK_GUI_INSTANCE.nk_context, "Remove")) {
            dk_state_change_inputmode(DK_REMOVE);
        }
        if (nk_button_label(DK_GUI_INSTANCE.nk_context, "Modify")) {
            dk_state_change_inputmode(DK_MODIFY);
        }
    }
    nk_end(DK_GUI_INSTANCE.nk_context);
}

static void _dk_gui_router() {
    int window_width = GetScreenWidth();

    if (nk_begin(DK_GUI_INSTANCE.nk_context, "Add router", nk_rect(window_width - 220, 175, 220, 195),
        NK_WINDOW_BORDER|NK_WINDOW_MINIMIZABLE)
    ) {
        nk_layout_row_begin(DK_GUI_INSTANCE.nk_context, NK_STATIC, 25, 2);
        nk_layout_row_push(DK_GUI_INSTANCE.nk_context, 50);
        nk_label(DK_GUI_INSTANCE.nk_context, "Name:", NK_TEXT_ALIGN_LEFT | NK_TEXT_ALIGN_MIDDLE);
        nk_layout_row_push(DK_GUI_INSTANCE.nk_context, 125);
        nk_edit_string_zero_terminated(DK_GUI_INSTANCE.nk_context, NK_EDIT_FIELD, DK_GUI_INSTANCE.add_router_mode.router_name, 50, nk_filter_default);
        nk_layout_row_dynamic(DK_GUI_INSTANCE.nk_context, 10, 1);
        nk_label(DK_GUI_INSTANCE.nk_context, "Color:", NK_TEXT_ALIGN_LEFT | NK_TEXT_ALIGN_BOTTOM);
        nk_layout_row_dynamic(DK_GUI_INSTANCE.nk_context, 100, 1);
        DK_GUI_INSTANCE.add_router_mode.router_color = ColorFromNuklearF(nk_color_picker(DK_GUI_INSTANCE.nk_context, ColorToNuklearF(DK_GUI_INSTANCE.add_router_mode.router_color), NK_RGB));
    }
    nk_end(DK_GUI_INSTANCE.nk_context);
}

static void _dk_gui_link() {
    int window_width = GetScreenWidth();

    if (nk_begin(DK_GUI_INSTANCE.nk_context, "Add link", nk_rect(window_width - 220, 175, 220, 190),
        NK_WINDOW_BORDER|NK_WINDOW_MINIMIZABLE)
    ) {
        nk_layout_row_begin(DK_GUI_INSTANCE.nk_context, NK_STATIC, 25, 2);
        nk_layout_row_push(DK_GUI_INSTANCE.nk_context, 50);
        nk_label(DK_GUI_INSTANCE.nk_context, "Cost:", NK_TEXT_ALIGN_LEFT | NK_TEXT_ALIGN_MIDDLE);
        nk_layout_row_push(DK_GUI_INSTANCE.nk_context, 125);
        nk_edit_string_zero_terminated(DK_GUI_INSTANCE.nk_context, NK_EDIT_FIELD, DK_GUI_INSTANCE.add_link_mode.cost_str, 20, nk_filter_decimal);
        DK_GUI_INSTANCE.add_link_mode.cost = atoi(DK_GUI_INSTANCE.add_link_mode.cost_str);
        nk_layout_row_dynamic(DK_GUI_INSTANCE.nk_context, 30, 1);
        nk_checkbox_label(DK_GUI_INSTANCE.nk_context, "Bidirectional ", (nk_bool*)&DK_GUI_INSTANCE.add_link_mode.bidirectional);

        if (!DK_GUI_INSTANCE.add_link_mode.bidirectional) {
            nk_layout_row_begin(DK_GUI_INSTANCE.nk_context, NK_STATIC, 25, 2);
            nk_layout_row_push(DK_GUI_INSTANCE.nk_context, 75);
            nk_label(DK_GUI_INSTANCE.nk_context, "Reverse cost:", NK_TEXT_ALIGN_LEFT | NK_TEXT_ALIGN_MIDDLE);
            nk_layout_row_push(DK_GUI_INSTANCE.nk_context, 100);
            nk_edit_string_zero_terminated(DK_GUI_INSTANCE.nk_context, NK_EDIT_FIELD, DK_GUI_INSTANCE.add_link_mode.reverse_cost_str, 20, nk_filter_decimal);
            DK_GUI_INSTANCE.add_link_mode.reverse_cost = atoi(DK_GUI_INSTANCE.add_link_mode.reverse_cost_str);
        }
        nk_layout_row_dynamic(DK_GUI_INSTANCE.nk_context, 30, 1);
    }
    nk_end(DK_GUI_INSTANCE.nk_context);
}

static void _dk_gui_modify_router() {
    int window_width = GetScreenWidth();

    if (nk_begin(DK_GUI_INSTANCE.nk_context, "Modify router", nk_rect(window_width - 220, 175, 220, 230),
        NK_WINDOW_BORDER|NK_WINDOW_MINIMIZABLE)
    ) {
        nk_layout_row_begin(DK_GUI_INSTANCE.nk_context, NK_STATIC, 25, 2);
        nk_layout_row_push(DK_GUI_INSTANCE.nk_context, 50);
        nk_label(DK_GUI_INSTANCE.nk_context, "Name:", NK_TEXT_ALIGN_LEFT | NK_TEXT_ALIGN_MIDDLE);
        nk_layout_row_push(DK_GUI_INSTANCE.nk_context, 125);
        nk_edit_string_zero_terminated(DK_GUI_INSTANCE.nk_context, NK_EDIT_FIELD, DK_GUI_INSTANCE.modify_mode.router_name, 50, nk_filter_default);
        nk_layout_row_dynamic(DK_GUI_INSTANCE.nk_context, 10, 1);
        nk_label(DK_GUI_INSTANCE.nk_context, "Color:", NK_TEXT_ALIGN_LEFT | NK_TEXT_ALIGN_BOTTOM);
        nk_layout_row_dynamic(DK_GUI_INSTANCE.nk_context, 100, 1);
        DK_GUI_INSTANCE.modify_mode.router_color = ColorFromNuklearF(nk_color_picker(DK_GUI_INSTANCE.nk_context, ColorToNuklearF(DK_GUI_INSTANCE.modify_mode.router_color), NK_RGB));
        nk_layout_row_dynamic(DK_GUI_INSTANCE.nk_context, 30, 1);
        if (nk_button_label(DK_GUI_INSTANCE.nk_context, "MODIFY")) {
            DK_GUI_INSTANCE.modify_mode.ready = true;
        }
    }
    nk_end(DK_GUI_INSTANCE.nk_context);
}

static void _dk_gui_modify_link() {
    int window_width = GetScreenWidth();

    if (nk_begin(DK_GUI_INSTANCE.nk_context, "Modify link", nk_rect(window_width - 220, 175, 220, 225),
        NK_WINDOW_BORDER|NK_WINDOW_MINIMIZABLE)
    ) {
        nk_layout_row_begin(DK_GUI_INSTANCE.nk_context, NK_STATIC, 25, 2);
        nk_layout_row_push(DK_GUI_INSTANCE.nk_context, 50);
        nk_label(DK_GUI_INSTANCE.nk_context, "Cost:", NK_TEXT_ALIGN_LEFT | NK_TEXT_ALIGN_MIDDLE);
        nk_layout_row_push(DK_GUI_INSTANCE.nk_context, 125);
        nk_edit_string_zero_terminated(DK_GUI_INSTANCE.nk_context, NK_EDIT_FIELD, DK_GUI_INSTANCE.modify_mode.cost_str, 20, nk_filter_decimal);
        DK_GUI_INSTANCE.modify_mode.cost = atoi(DK_GUI_INSTANCE.modify_mode.cost_str);
        nk_layout_row_dynamic(DK_GUI_INSTANCE.nk_context, 30, 1);
        nk_checkbox_label(DK_GUI_INSTANCE.nk_context, "Bidirectional ", (nk_bool*)&DK_GUI_INSTANCE.modify_mode.bidirectional);

        if (!DK_GUI_INSTANCE.modify_mode.bidirectional) {
            nk_layout_row_begin(DK_GUI_INSTANCE.nk_context, NK_STATIC, 25, 2);
            nk_layout_row_push(DK_GUI_INSTANCE.nk_context, 75);
            nk_label(DK_GUI_INSTANCE.nk_context, "Reverse cost:", NK_TEXT_ALIGN_LEFT | NK_TEXT_ALIGN_MIDDLE);
            nk_layout_row_push(DK_GUI_INSTANCE.nk_context, 100);
            nk_edit_string_zero_terminated(DK_GUI_INSTANCE.nk_context, NK_EDIT_FIELD, DK_GUI_INSTANCE.modify_mode.reverse_cost_str, 20, nk_filter_decimal);
            DK_GUI_INSTANCE.modify_mode.reverse_cost = atoi(DK_GUI_INSTANCE.modify_mode.reverse_cost_str);
        }

        nk_layout_row_dynamic(DK_GUI_INSTANCE.nk_context, 30, 1);
        if (nk_button_label(DK_GUI_INSTANCE.nk_context, "MODIFY")) {
            DK_GUI_INSTANCE.modify_mode.ready = true;
        }
    }
    nk_end(DK_GUI_INSTANCE.nk_context);
}

static void _dk_gui_modify() {
    if (DK_GUI_INSTANCE.modify_mode.ready) {
        return;
    }
    if (DK_GUI_INSTANCE.modify_mode.current_modify == -1) {
        DK_GUI_INSTANCE.modify_mode.ready = false;
        DK_GUI_INSTANCE.modify_mode.request = false;
        DK_GUI_INSTANCE.modify_mode.router_color = BLACK;
        strcpy(DK_GUI_INSTANCE.modify_mode.router_name, "Router");
        DK_GUI_INSTANCE.modify_mode.bidirectional = true;
        strcpy(DK_GUI_INSTANCE.modify_mode.cost_str, "1");
        strcpy(DK_GUI_INSTANCE.modify_mode.reverse_cost_str, "1");

        return;
    }

    if (DK_GUI_INSTANCE.modify_mode.request) {
        DK_GUI_INSTANCE.modify_mode.request = false;

        if (!DK_GUI_INSTANCE.modify_mode.modify_is_line) {
            dk_Router* router = &VX_VD(&DK_STATE_INSTANCE.routers)[DK_GUI_INSTANCE.modify_mode.current_modify];

            DK_GUI_INSTANCE.modify_mode.router_color = router->color;
            strcpy(DK_GUI_INSTANCE.modify_mode.router_name, router->identifier);
        } else {
            dk_Link* link = &VX_VD(&DK_STATE_INSTANCE.links)[DK_GUI_INSTANCE.modify_mode.current_modify];

            DK_GUI_INSTANCE.add_link_mode.bidirectional = link->bidirectional;
            itoa(link->cost, DK_GUI_INSTANCE.add_link_mode.cost_str, 10);
            itoa(link->cost_reverse, DK_GUI_INSTANCE.add_link_mode.reverse_cost_str, 10);
        }
    }

    if (!DK_GUI_INSTANCE.modify_mode.modify_is_line) {
        _dk_gui_modify_router();
    } else {
        _dk_gui_modify_link();
    }
}

void dk_gui_init() {
    DK_GUI_INSTANCE.nk_context = InitNuklear(FONT_SIZE);

    DK_GUI_INSTANCE.calculate.start = -1;
    DK_GUI_INSTANCE.calculate.end = -1;
    DK_GUI_INSTANCE.calculate.run = false;
    DK_GUI_INSTANCE.calculate.select_start_required = false;
    DK_GUI_INSTANCE.calculate.select_end_required = false;

    DK_GUI_INSTANCE.add_router_mode.router_color = BLACK;
    strcpy(DK_GUI_INSTANCE.add_router_mode.router_name, "Router");

    DK_GUI_INSTANCE.add_link_mode.bidirectional = true;
    strcpy(DK_GUI_INSTANCE.add_link_mode.cost_str, "1");
    strcpy(DK_GUI_INSTANCE.add_link_mode.reverse_cost_str, "1");

    DK_GUI_INSTANCE.modify_mode.current_modify = -1;
    DK_GUI_INSTANCE.modify_mode.modify_is_line = false;
    DK_GUI_INSTANCE.modify_mode.ready = false;
    DK_GUI_INSTANCE.modify_mode.request = false;
    DK_GUI_INSTANCE.modify_mode.router_color = BLACK;
    strcpy(DK_GUI_INSTANCE.modify_mode.router_name, "Router");
    DK_GUI_INSTANCE.modify_mode.bidirectional = true;
    strcpy(DK_GUI_INSTANCE.modify_mode.cost_str, "1");
    strcpy(DK_GUI_INSTANCE.modify_mode.reverse_cost_str, "1");
}

void dk_gui_logic() {
    UpdateNuklear(DK_GUI_INSTANCE.nk_context);

    _dk_gui_calculate();
    _dk_gui_common();
    if (DK_STATE_INSTANCE.input_mode == DK_ADD_ROUTER) {
        _dk_gui_router();
    } else if (DK_STATE_INSTANCE.input_mode == DK_ADD_LINK) {
        _dk_gui_link();
    } else if (DK_STATE_INSTANCE.input_mode == DK_MODIFY) {
        _dk_gui_modify();
    }
}

void dk_gui_draw() {
    DrawNuklear(DK_GUI_INSTANCE.nk_context);
}