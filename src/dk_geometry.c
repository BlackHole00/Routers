#include "dk_geometry.h"

#include <math.h>

bool dk_point_is_near_point(dk_Point p1, dk_Point p2, f32 radius) {
    return (
        (p1.x <= p2.x + radius) &&
        (p1.x >= p2.x - radius) &&
        (p1.y <= p2.y + radius) &&
        (p1.y >= p2.y - radius)
    ); 
}

f32 dk_point_to_point_distance(dk_Point p1, dk_Point p2) {
    return sqrtf(powf((f32)(p1.x - p2.x), 2.0) + powf((f32)(p1.y - p2.y), 2.0));
}

bool dk_point_is_inside_line(dk_Point p, dk_Point line_start, dk_Point line_end, f32 delta) {
    f32 distance = dk_point_to_point_distance(line_start, p) + dk_point_to_point_distance(p, line_end);
    f32 ipo = dk_point_to_point_distance(line_start, line_end);
    return distance <= (ipo + (f64)(delta) / 5.0) && distance >= (ipo - (f64)(delta) / 5.0);
}