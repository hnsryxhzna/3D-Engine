#ifndef RENDER_H
#define RENDER_H

#include "vector_math.h"

extern int depth_test_enabled;


typedef struct {
    int i0;
    int i1;
    int i2;
    vec3 color;
} triangle;

typedef struct {
    int x;
    int y;
    int z;
    int k;
    vec3 color;
} square;

typedef struct {
    const vec3* vectors;
    const triangle* triangles;
    int nvector;
    int ntriangle;
} mesh;

typedef struct {
    int W;
    int H;
    unsigned char* color;
    float* depth;
} frame_buffer;

frame_buffer new_frame_buffer(int W, int H);
void fb_free(frame_buffer *fb);
void fb_clear(frame_buffer *fb, unsigned char r, unsigned char g, unsigned char b);
void fb_write_ppm(char *file_path, frame_buffer *fb);
void fb_set_pixel(frame_buffer *fb, int x, int y, unsigned char r, unsigned char g, unsigned char b);
void draw_dot(frame_buffer *fb, int x, int y, unsigned char r, unsigned char g, unsigned char b);
void draw_line(frame_buffer *fb, int x0, int y0, int x1, int y1, unsigned char r, unsigned char g, unsigned char b);
int project_vertex(mat4 mvp, vec3 v, int W, int H, int *px, int *py);
void render_wireframe(frame_buffer *fb, mat4 mvp, vec3 cube_vertices[8], int cube_edges[12][2]);

int project_to_screen(mat4 mvp, vec3 vec, int W, int H, vec3 *out);
void draw_triangle(frame_buffer *fb, vec3 v0, vec3 v1, vec3 v2, vec3 color);
void render_mesh(frame_buffer *fb, const mesh *m, mat4 mvp);

void fb_write_depth_ppm(const char *path, const frame_buffer *fb);

#endif
