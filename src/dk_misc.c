#include "dk_misc.h"

Color dk_color_mix(Color c1, Color c2) {
    Color c;
    c.r = (c1.r + c2.r) / 2;
    c.g = (c1.g + c2.g) / 2;
    c.b = (c1.b + c2.b) / 2;
    c.a = (c1.a + c2.a) / 2;

    return c;
}