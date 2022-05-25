#include <stdio.h>
#include <raylib.h>
#include <raygui.h>
#include <vx_utils.h>
#include "dk_matrix.h"

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Routers");
    SetTargetFPS(60);

    dk_state_init();

    while (!WindowShouldClose()) {
        dk_state_logic();

        BeginDrawing();
        dk_state_draw();
        EndDrawing();
    }

    CloseWindow();

    dk_state_free();
    vx_memory_print_state();

    return 0;
}
