#pragma once

#include <vx_utils.h>
#include <raylib.h>

typedef Vector2 dk_Point;

f32 dk_point_to_point_distance(dk_Point p1, dk_Point p2);

bool dk_point_is_near_point(dk_Point p1, dk_Point p2, f32 radius);
bool dk_point_is_inside_line(dk_Point p, dk_Point line_start, dk_Point line_end, f32 delta);