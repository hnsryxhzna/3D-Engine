#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "render.h"

#define CULL_BACKFACES 1
int depth_test_enabled = 1;
int light_enabled = 1;
vec3 light_direction = {1.0f, 1.0f, 1.0f};

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

int project_to_screen(mat4 mvp, vec3 vec, int W, int H, vec3* out) {
    vec4 clip = mat4_mul_vec4(mvp, vector4(vec.x, vec.y, vec.z, 1.0f));
    if (clip.k <= 0.0f) {
        return 0;
    }
    float inv_w = 1.0f / clip.k;
    out->x = (clip.x * inv_w + 1.0f) * 0.5f * W;
    out->y = (1.0f - clip.y * inv_w) * 0.5f * H;
    out->z = clip.z * inv_w;
    return 1;
}

static float edge(vec3 a, vec3 b, float px, float py) {
    return (b.x - a.x) * (py - a.y) - (b.y - a.y) * (px - a.x);
}

void draw_triangle(frame_buffer *fb, vec3 v0, vec3 v1, vec3 v2, vec3 color) {
    float area = edge(v0, v1, v2.x, v2.y);
    if (area == 0.0f) {
        return;
    } else if (CULL_BACKFACES && area > 0.0f) {
        return;
    }

    int min_x = (int) floorf(fminf(v0.x, fminf(v1.x, v2.x)));
    int max_x = (int) ceilf (fmaxf(v0.x, fmaxf(v1.x, v2.x)));
    int min_y = (int) floorf(fminf(v0.y, fminf(v1.y, v2.y)));
    int max_y = (int) ceilf (fmaxf(v0.y, fmaxf(v1.y, v2.y)));
    if (min_x < 0) min_x = 0;
    if (min_y < 0) min_y = 0;
    if (max_x > fb->W - 1) max_x = fb->W - 1;
    if (max_y > fb->H - 1) max_y = fb->H - 1;

    // unsigned char r = (unsigned char) (color.x * 255.0f);
    // unsigned char g = (unsigned char) (color.y * 255.0f);
    // unsigned char b = (unsigned char) (color.z * 255.0f);

    unsigned char r = (unsigned char) (color.x);
    unsigned char g = (unsigned char) (color.y);
    unsigned char b = (unsigned char) (color.z);

    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            float px = x + 0.5f;
            float py = y + 0.5f;

            float w0 = edge(v1, v2, px, py) / area;
            float w1 = edge(v2, v0, px, py) / area;
            float w2 = edge(v0, v1, px, py) / area;

            if (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f) {
                float z = w0 * v0.z + w1 * v1.z + w2 * v2.z;
                size_t idx = (size_t)y * fb->W + x;

                if (!depth_test_enabled || z < fb->depth[idx]) {
                    fb->depth[idx] = z;
                    fb_set_pixel(fb, x, y, r, g, b);
                }
            }
        }
    }
}

void render_mesh(frame_buffer *fb, const mesh *m, mat4 model, mat4 vp) {
    mat4 mvp = mat4_mul(vp, model);

    vec3 *world = malloc(sizeof(vec3) * m->nvector);
    vec3 *screen = malloc(sizeof(vec3) * m->nvector);
    int *ok = malloc(sizeof(int) * m->nvector);
    if (world == NULL || screen == NULL || ok == NULL) {
        free(world);
        free(screen);
        free(ok);
        return;
    }

    for (int i = 0; i < m->nvector; i++) {
        world[i] = mat4_mul_point(model, m->vectors[i]);
        ok[i] = project_to_screen(mvp, m->vectors[i], fb->W, fb->H, &screen[i]);
    }

    for (int t = 0; t < m->ntriangle; t++) {
        triangle tri = m->triangles[t];
        if (!ok[tri.i0] || !ok[tri.i1] || !ok[tri.i2]) {
            continue;
        }
        
        vec3 color = tri.color;
        if (light_enabled) {
            float l = flat_indensity(world[tri.i0], world[tri.i1], world[tri.i2]);
            color = vector3_scale(color, l);
        }

        draw_triangle(fb, screen[tri.i0], screen[tri.i1], screen[tri.i2], tri.color);
    }

    free(world);
    free(screen);
    free(ok);
}

void fb_write_depth_ppm(const char *path, const frame_buffer *fb) {
    float lo = INFINITY;
    float hi = -INFINITY;

    for (int i = 0; i < fb->W * fb->H; i++) {
        float d = fb->depth[i];
        if (isinf(d)) {
            continue;
        }

        if (d < lo) {
            lo = d;
        }
        if (d > hi) {
            hi = d;
        }
    }

    FILE *f = fopen(path, "wb");
    if (f == NULL) {
        perror(path);
        return;
    }
    fprintf(f, "P6\n%d %d\n255\n", fb->W, fb->H);

    for (int i = 0; i < fb->W * fb->H; i++) {
        float d = fb->depth[i];
        unsigned char g = 0;
        if (!isinf(d) && hi > lo) {
            g = (unsigned char) (255.0f * (1.0f - (d - lo) / (hi - lo)));
        }

        unsigned char px[3] = {g, g, g};
        fwrite(px, 1, 3, f);
    }

    fclose(f);
}

float flat_indensity(vec3 a, vec3 b, vec3 c) {
    vec3 normal = vec3_norm(vec3_corss_product(v3_sub(a, b), v3_sub(a, c)));
    vec3 light = light_direction;

    float diffuse = vec3_dot_product(normal, light);

    return diffuse;
}
