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
    if (!f) {
        perror(file_path);
        return;
    }

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

void draw_line(frame_buffer *fb, int x0, int y0, int x1, int y1, unsigned char r, unsigned char g, unsigned char b) {
    int dx = abs(x1 - x0), sx = (x0 < x1) ? 1 : -1;
    int dy = -abs(y1 - y0), sy = (y0 < y1) ? 1 : -1;
    int err = dx + dy;

    while (1) {
        fb_set_pixel(fb, x0, y0, r, g, b);
        if (x0 == x1 && y0 == y1) {
            break;
        }
        int e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
    }
}

int project_vertex(mat4 mvp, vec3 v, int W, int H, int *px, int *py) {
    vec4 clip = mat4_mul_vec4(mvp, vector4(v.x, v.y, v.z, 1.0f));
    if (clip.k <= 0.0f) {
        return 0;
    }

    float inv_w = 1.0f / clip.k;
    float ndc_x = clip.x * inv_w;
    float ndc_y = clip.y * inv_w;
    *px = (int) ((ndc_x + 1.0f) * 0.5f * W);
    *py = (int) ((1.0f - ndc_y) * 0.5f * H);
    return 1;
}

void render_wireframe(frame_buffer *fb, mat4 mvp, vec3 cube_vertices[8], int cube_edges[12][2]) {
    int px[8], py[8], ok[8];

    for (int i = 0; i < 8; i++) {
        ok[i] = project_vertex(mvp, cube_vertices[i], fb->W, fb->H, &px[i], &py[i]);
    }

    for (int e = 0; e < 12; e++) {
        int a = cube_edges[e][0];
        int b = cube_edges[e][1];
        if (!ok[a] || !ok[b]) {
            continue;
        }

        draw_line(fb, px[a], py[a], px[b], py[b], 255, 255, 255);
    }
}