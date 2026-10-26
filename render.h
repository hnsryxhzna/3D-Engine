#ifndef RENDER_H
#define RENDER_H

#include "vector_math.h"

typedef struct {
    int x;
    int y;
    int z;
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
    int nvector;
    const triangle* triangles;
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

#endif
