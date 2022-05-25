#include "dk_state.h"

#include <stdlib.h>
#include <string.h>

#include "dk_geometry.h"
#include "dk_gui.h"
#include "dk_misc.h"

dk_State DK_STATE_INSTANCE;

static bool _is_point_over_router(dk_Point point) {
    bool ok = false;
    VX_VECTOR_FOREACH(dk_Router, elem, &DK_STATE_INSTANCE.routers,
        if (dk_point_is_near_point(point, elem.position, DK_ROUTER_RADIUS * 2.0f)) {
            ok = true;
            break;
        }
    )

    return ok;
}

static i32 _get_hovered_router(dk_Point point) {
    VX_VECTOR_FOREACH(dk_Router, elem, &DK_STATE_INSTANCE.routers,
        if (dk_point_is_near_point(point, elem.position, DK_ROUTER_RADIUS)) {
            return I;
        }
    )

    return -1;
}

static i32 _get_hovered_link(dk_Point point) {
    VX_VECTOR_FOREACH(dk_Link, elem, &DK_STATE_INSTANCE.links,
        Vector2 start = VX_VD(&DK_STATE_INSTANCE.routers)[elem.to].position;
        Vector2 end = VX_VD(&DK_STATE_INSTANCE.routers)[elem.from].position;

        if (dk_point_is_inside_line(point, start, end, 5.0)) {
            return I;
        }
    )

    return -1;
}

static bool _does_link_exists(dk_Link link) {
    VX_VECTOR_FOREACH(dk_Link, elem, &DK_STATE_INSTANCE.links,
        if (elem.to == link.to && elem.from == link.from ) {
            return true;
        }
    )

    return false;
}

static void _remove_router(i32 idx) {
    dk_Router* router = &VX_VD(&DK_STATE_INSTANCE.routers)[idx];

    VX_T(i32, vx_Vector) bad_links = VX_T(i32, vx_vector_new)();

    VX_VECTOR_FOREACH(dk_Link, link, &DK_STATE_INSTANCE.links,
        if (link.to == idx || link.from == idx) {
            VX_T(i32, vx_vector_push)(&bad_links, I);
        }
    )

    i32 deleted_elems = 0;
    VX_VECTOR_FOREACH(i32, elem, &bad_links,
        VX_T(dk_Link, vx_vector_remove)(&DK_STATE_INSTANCE.links, elem - deleted_elems);
        deleted_elems++;
    )

    dk_router_free(router);
    VX_T(dk_Router, vx_vector_remove)(&DK_STATE_INSTANCE.routers, idx);

    /* All the links are still referencing to the old router indexes. If needed the reference is updated. */
    VX_VECTOR_FOREACHMUT(dk_Link, link, &DK_STATE_INSTANCE.links,
        if (link->to > idx) {
            link->to--;
        }
        if (link->from > idx) {
            link->from--;
        }
    )

    VX_T(i32, vx_vector_free)(&bad_links);

    VX_T(i32, vx_vector_clear)(&DK_STATE_INSTANCE.optimal_links);
    DK_STATE_INSTANCE.optimal_cost = -1;
    DK_GUI_INSTANCE.calculate.start = -1;
    DK_GUI_INSTANCE.calculate.end = -1;
}

static void _remove_link(i32 idx) {
    VX_T(dk_Link, vx_vector_remove)(&DK_STATE_INSTANCE.links, idx);

    VX_T(i32, vx_vector_clear)(&DK_STATE_INSTANCE.optimal_links);
    DK_STATE_INSTANCE.optimal_cost = -1;
    DK_GUI_INSTANCE.calculate.start = -1;
    DK_GUI_INSTANCE.calculate.end = -1;
}

static void _calculate_map() {
    DK_STATE_INSTANCE.map.data = vx_srealloc(DK_STATE_INSTANCE.map.data, DK_STATE_INSTANCE.routers.length * DK_STATE_INSTANCE.routers.length * sizeof(i32));
    DK_STATE_INSTANCE.map.size = DK_STATE_INSTANCE.routers.length;

    for (int i = 0; i < DK_STATE_INSTANCE.map.size * DK_STATE_INSTANCE.map.size; i++) {
        DK_STATE_INSTANCE.map.data[i] = DK_NO_LINK;
    }

    VX_VECTOR_FOREACH(dk_Link, elem, &DK_STATE_INSTANCE.links,
        DK_MATRIX_DATA(&DK_STATE_INSTANCE.map, elem.from, elem.to) = elem.cost;

        if (elem.bidirectional) {
            DK_MATRIX_DATA(&DK_STATE_INSTANCE.map, elem.to, elem.from) = elem.cost;
        } else {
            DK_MATRIX_DATA(&DK_STATE_INSTANCE.map, elem.to, elem.from) = elem.cost_reverse;
        }
    )

    DK_STATE_INSTANCE.optimal_cost = dk_dijkstra_path(&DK_STATE_INSTANCE.map, DK_GUI_INSTANCE.calculate.start, DK_GUI_INSTANCE.calculate.end, &DK_STATE_INSTANCE.optimal_links);
}

void dk_state_init() {
    DK_STATE_INSTANCE.routers = VX_T(dk_Router, vx_vector_new)();
    DK_STATE_INSTANCE.links = VX_T(dk_Link, vx_vector_new)();
    DK_STATE_INSTANCE.optimal_links = VX_T(i32, vx_vector_new)();

    DK_STATE_INSTANCE.start = 0;
    DK_STATE_INSTANCE.end = 0;
    DK_STATE_INSTANCE.optimal_cost = -1;
    DK_STATE_INSTANCE.input_mode = DK_ADD_ROUTER;
    DK_STATE_INSTANCE.last_selected = -1;

    DK_STATE_INSTANCE.map = dk_matrix_from_data(0, vx_smalloc(0));

    dk_gui_init();
}

void dk_state_free() {
    VX_VECTOR_FOREACH(dk_Router, elem, &DK_STATE_INSTANCE.routers,
        dk_router_free(&elem);
    )

    VX_T(dk_Router, vx_vector_free)(&DK_STATE_INSTANCE.routers);
    VX_T(dk_Link,   vx_vector_free)(&DK_STATE_INSTANCE.links);
    VX_T(i32,       vx_vector_free)(&DK_STATE_INSTANCE.optimal_links);
}

void dk_state_logic() {
    dk_gui_logic();

    if (DK_GUI_INSTANCE.calculate.run) {
        if (DK_GUI_INSTANCE.calculate.end != -1 && DK_GUI_INSTANCE.calculate.start != -1) {
            _calculate_map();
        }

        DK_GUI_INSTANCE.calculate.run = false;
    }

    if (nk_window_is_any_hovered(DK_GUI_INSTANCE.nk_context)) {
        return;
    }

    if (IsKeyPressed(KEY_ONE)) {
        dk_state_change_inputmode(DK_ADD_ROUTER);
    } else if (IsKeyPressed(KEY_TWO)) {
        dk_state_change_inputmode(DK_ADD_LINK);
    } else if (IsKeyPressed(KEY_THREE)) {
        dk_state_change_inputmode(DK_REMOVE);
    }

    if (DK_GUI_INSTANCE.calculate.select_start_required) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            i32 router = _get_hovered_router(GetMousePosition());
            if (router != -1) {
                DK_GUI_INSTANCE.calculate.start = router;
                DK_GUI_INSTANCE.calculate.select_start_required = false;
            }
        }

        return;
    }
    if (DK_GUI_INSTANCE.calculate.select_end_required) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            i32 router = _get_hovered_router(GetMousePosition());
            if (router != -1) {
                DK_GUI_INSTANCE.calculate.end = router;
                DK_GUI_INSTANCE.calculate.select_end_required = false;
            }
        }

        return;
    }

    switch(DK_STATE_INSTANCE.input_mode) {
        case DK_ADD_ROUTER: {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mouse_pos = GetMousePosition();
                if (!_is_point_over_router(mouse_pos)) {
                    dk_Router router = dk_router_from_identifier(DK_GUI_INSTANCE.add_router_mode.router_name);
                    router.position = mouse_pos; 
                    router.color = DK_GUI_INSTANCE.add_router_mode.router_color;
                    router.color.a = 255;

                    VX_T(dk_Router, vx_vector_push)(&DK_STATE_INSTANCE.routers, router);
                }
            }

            break;
        }
        case DK_ADD_LINK: {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                i32 router = _get_hovered_router(GetMousePosition());
                if (router != -1) {
                    if (DK_STATE_INSTANCE.last_selected == -1) {
                        DK_STATE_INSTANCE.last_selected = router;
                    } else {
                        dk_Link link = (dk_Link){
                            .cost_reverse = DK_GUI_INSTANCE.add_link_mode.reverse_cost,
                            .cost = DK_GUI_INSTANCE.add_link_mode.cost,
                            .from = DK_STATE_INSTANCE.last_selected,
                            .to = router,
                            .bidirectional = DK_GUI_INSTANCE.add_link_mode.bidirectional
                        };

                        if (!_does_link_exists(link)) {
                            VX_T(dk_Link, vx_vector_push)(&DK_STATE_INSTANCE.links, link);

                            DK_STATE_INSTANCE.last_selected = -1;
                        }

                    }
                }
            }

            break;
        }
        case DK_REMOVE: {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mouse_pos = GetMousePosition();
                i32 idx = _get_hovered_router(mouse_pos);
                if (idx != -1) {
                    _remove_router(idx);
                    break;
                }

                idx = _get_hovered_link(mouse_pos);
                if (idx != -1) {
                    _remove_link(idx);
                }
            }

            break;
        }
        case DK_MODIFY: {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                Vector2 mouse_pos = GetMousePosition();
                i32 idx = _get_hovered_router(mouse_pos);
                if (idx != -1) {
                    DK_GUI_INSTANCE.modify_mode.current_modify = idx;
                    DK_GUI_INSTANCE.modify_mode.modify_is_line = false;
                    DK_GUI_INSTANCE.modify_mode.request = true;

                    break;
                }

                idx = _get_hovered_link(mouse_pos);
                if (idx != -1) {
                    DK_GUI_INSTANCE.modify_mode.current_modify = idx;
                    DK_GUI_INSTANCE.modify_mode.modify_is_line = true;
                    DK_GUI_INSTANCE.modify_mode.request = true;
                }
            } else {
                if (DK_GUI_INSTANCE.modify_mode.ready) {
                    if (!DK_GUI_INSTANCE.modify_mode.modify_is_line) {
                        dk_Router* router = &VX_VD(&DK_STATE_INSTANCE.routers)[DK_GUI_INSTANCE.modify_mode.current_modify];

                        Vector2 oldpos = router->position;
                        dk_router_free(router);

                        *router = dk_router_from_identifier(DK_GUI_INSTANCE.modify_mode.router_name);
                        router->color = DK_GUI_INSTANCE.modify_mode.router_color;
                        router->position = oldpos;
                    } else {
                        dk_Link* link = &VX_VD(&DK_STATE_INSTANCE.links)[DK_GUI_INSTANCE.modify_mode.current_modify];
                        link->bidirectional = DK_GUI_INSTANCE.modify_mode.bidirectional;
                        link->cost = DK_GUI_INSTANCE.modify_mode.cost;
                        link->cost_reverse = DK_GUI_INSTANCE.modify_mode.reverse_cost;
                    }

                    DK_GUI_INSTANCE.modify_mode.ready = false;
                }
            }
        }
        default: {
            break;
        }
    }
}

void dk_state_draw() {
    ClearBackground(RAYWHITE);

    if (DK_STATE_INSTANCE.last_selected != -1) {
        Vector2 pos = VX_VD(&DK_STATE_INSTANCE.routers)[DK_STATE_INSTANCE.last_selected].position;
        Vector2 mouse_pos = GetMousePosition();

        DrawLine(pos.x, pos.y, mouse_pos.x, mouse_pos.y, GRAY);
    }

    dk_Router* prev = &VX_VD(&DK_STATE_INSTANCE.routers)[DK_GUI_INSTANCE.calculate.start];
    VX_VECTOR_FOREACH(i32, elem, &DK_STATE_INSTANCE.optimal_links,
        dk_Router* current = &VX_VD(&DK_STATE_INSTANCE.routers)[elem];

        DrawLineEx(current->position, prev->position, 3, GREEN);

        prev = current;
    )

    VX_VECTOR_FOREACH(dk_Link, elem, &DK_STATE_INSTANCE.links,
        dk_Router* start = &VX_VD(&DK_STATE_INSTANCE.routers)[elem.from];
        dk_Router* end   = &VX_VD(&DK_STATE_INSTANCE.routers)[elem.to];

        DrawLine(start->position.x, start->position.y, end->position.x, end->position.y, dk_color_mix(start->color, end->color));
    )

    if (DK_GUI_INSTANCE.calculate.start != -1) {
        dk_Router* start = &VX_VD(&DK_STATE_INSTANCE.routers)[DK_GUI_INSTANCE.calculate.start];

        DrawCircle(start->position.x, start->position.y, DK_ROUTER_RADIUS + 3, PURPLE);
    }
    if (DK_GUI_INSTANCE.calculate.end != -1) {
        dk_Router* end = &VX_VD(&DK_STATE_INSTANCE.routers)[DK_GUI_INSTANCE.calculate.end];

        DrawCircle(end->position.x, end->position.y, DK_ROUTER_RADIUS + 3, ORANGE);
    }

    VX_VECTOR_FOREACH(dk_Router, elem, &DK_STATE_INSTANCE.routers,
        DrawCircle(elem.position.x, elem.position.y, DK_ROUTER_RADIUS, elem.color);
    )

    i32 router = _get_hovered_router(GetMousePosition());
    if (router != -1) {
        Color color = VX_VD(&DK_STATE_INSTANCE.routers)[router].color;
        color.a = 150;
        DrawText(VX_VD(&DK_STATE_INSTANCE.routers)[router].identifier, 
            VX_VD(&DK_STATE_INSTANCE.routers)[router].position.x + DK_ROUTER_RADIUS,
            VX_VD(&DK_STATE_INSTANCE.routers)[router].position.y - DK_ROUTER_RADIUS,
            FONT_SIZE,
            color
        );
    } else {
        i32 link_idx = _get_hovered_link(GetMousePosition());
        if (link_idx != -1) {
            dk_Link* link = &VX_VD(&DK_STATE_INSTANCE.links)[link_idx];

            static char tmp[256];
            snprintf(tmp, 256, 
                "From: %s\nTo: %s\nCost: %d\n", 
                VX_VD(&DK_STATE_INSTANCE.routers)[link->from].identifier,
                VX_VD(&DK_STATE_INSTANCE.routers)[link->to].identifier,
                link->cost
            );
            if (!link->bidirectional) {
                static char num[20];
                itoa(link->cost_reverse, num, 10);

                strcat(tmp, "Reverse cost: ");
                strcat(tmp, num);
            }

            DrawText(tmp, 
                GetMousePosition().x + 12,
                GetMousePosition().y,
                FONT_SIZE,
                (Color){.r = 0, .g = 0, .b = 0, .a = 150}
            );
        }
    }

    dk_gui_draw();
}

void dk_state_change_inputmode(dk_StateInputMode mode) {
    DK_STATE_INSTANCE.last_selected = -1;
    DK_GUI_INSTANCE.modify_mode.current_modify = -1;

    DK_STATE_INSTANCE.input_mode = mode;
    DK_GUI_INSTANCE.calculate.select_end_required = false;
    DK_GUI_INSTANCE.calculate.select_start_required = false;
}

void dk_stateinputmode_to_string(dk_StateInputMode mode, char* buffer, int size, int start_idx) {
    switch(mode) {
        case DK_ADD_ROUTER: {
            strcpy_s(buffer + (start_idx * sizeof(char)), size - start_idx, "ADD ROUTER");
            break;
        }
        case DK_ADD_LINK: {
            strcpy_s(buffer + (start_idx * sizeof(char)), size - start_idx, "ADD LINK");
            break;
        }
        case DK_REMOVE: {
            strcpy_s(buffer + (start_idx * sizeof(char)), size - start_idx, "REMOVE");
            break;
        }
        case DK_MODIFY: {
            strcpy_s(buffer + (start_idx * sizeof(char)), size - start_idx, "MODIFY");
            break;
        }
        default: {
            strcpy(buffer, "");
            break;
        }
    }
}