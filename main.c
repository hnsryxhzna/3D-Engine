#include <stdio.h>

#include "vector_math.h"
#include "render.h"

int main(void) {
    frame_buffer fb = new_frame_buffer(512, 512);
    fb_clear(&fb, 50, 11, 90);
    fb_write_ppm("test.ppm", &fb);
    fb_free(&fb);

    return 0;
}
