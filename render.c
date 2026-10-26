#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "render.h"

frame_buffer new_frame_buffer(int W, int H) {
    frame_buffer fb;
    fb.W = W;
    fb.H = H;
    fb.color = (unsigned char*) malloc((size_t) W * H * 3);
    fb.depth = (float*) malloc((size_t) W * H * sizeof(float));
    return fb;
}

void fb_free(frame_buffer *fb) { 
    free(fb->color);
    free(fb->depth);
    fb->color = NULL;
    fb->depth = NULL;
}

void fb_clear(frame_buffer *fb, unsigned char r, unsigned char g, unsigned char b) {
    for (int i = 0; i < fb->W * fb->H; i++){
        fb->color[i * 3 + 0] = r;
        fb->color[i * 3 + 1] = g;
        fb->color[i * 3 + 2] = b;
        fb->depth[i] = INFINITY;
    }
}

void fb_write_ppm(char *file_path, frame_buffer *fb) {
    FILE *f = fopen(file_path, "wb");
    fprintf(f, "P6\n%d %d\n255\n", fb->W, fb->H);
    fwrite(fb->color, 1, (size_t) fb->W * fb->H * 3, f);
    fclose(f);
}

void fb_set_pixel(frame_buffer *fb, int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    if (x < 0 || y < 0 || x > fb->W || y > fb->H) {
        return;
    }

    size_t i = ((size_t) y * fb->W + x) * 3;
    fb->color[i + 0] = r;
    fb->color[i + 1] = g;
    fb->color[i + 2] = b;
}

void draw_dot(frame_buffer *fb, int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            fb_set_pixel(fb, x + dx, y + dy, r, g, b);
        }
    }
}
