#include <stdio.h>

#include "vector_math.h"
#include "render.h"

#define WIDTH 512
#define HEIGHT 512
#define TWO_PI 6.28318530718f

int main(void) {
    frame_buffer fb = new_frame_buffer(WIDTH, HEIGHT);
    fb_clear(&fb, 50, 11, 90);

    vec3 cube[8] = {
        {-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1}, {-1, -1, 1}, {1, -1, 1}, {1, 1, 1}, {-1, 1, 1}
    };

    mat4 model = mat4_mul(mat4_rotate_y(0.6f), mat4_rotate_x(0.4f));
    mat4 view = mat4_look_at(vector3(0, 0, 5), vector3(0, 0, 0), vector3(0, 1, 0));

    mat4 proj = mat4_perspective(60.0f * (float) (TWO_PI / 2) / 180.0f, (float) WIDTH / HEIGHT, 0.1f, 100.0f);

    mat4 mv = mat4_mul(view, model);
    mat4 mvp = mat4_mul(proj, mv);

    float min_x = 1e9f;
    float max_x = -1e9f;
    float min_y = 1e9f;
    float max_y = -1e9f;
    float min_z = 1e9f;
    float max_z = -1e9f;

    for (int i = 0; i < 8; i++) {
        vec3 v = cube[i];

        vec3 cam = mat4_mul_point(mv, v);

        vec4 clip = mat4_mul_vec4(mvp, vector4(v.x, v.y, v.z, 1.0f));
        if (clip.k <= 0.0f) {
            fprintf(stderr, "v%d is behind the camera, skipped\n", i);
            continue;
        }

        float inv_w = 1.0f / clip.k;
        vec3 ndc = vector3(clip.x * inv_w, clip.y * inv_w, clip.z * inv_w);

        if (ndc.x < min_x) min_x = ndc.x;
        if (ndc.x > max_x) max_x = ndc.x;
        if (ndc.y < min_y) min_y = ndc.y;
        if (ndc.y > max_y) max_y = ndc.y;
        if (ndc.z < min_z) min_z = ndc.z;
        if (ndc.z > max_z) max_z = ndc.z;

        int px = (int)((ndc.x + 1.0f) * 0.5f * WIDTH);
        int py = (int)((1.0f - ndc.y) * 0.5f * HEIGHT);

        fprintf(stderr, "v%d cam(%6.3f %6.3f %6.3f) clip(%6.3f %6.3f %6.3f %6.3f) " "ndc(%6.3f %6.3f %6.3f) px(%d %d)\n", i, cam.x, cam.y, cam.z, clip.x, clip.y, clip.z, clip.k, ndc.x, ndc.y, ndc.z, px, py);
        draw_dot(&fb, px, py, 255, 255, 255);
    }

    fprintf(stderr, "NDC ranges: x[%.3f, %.3f] y[%.3f, %.3f] z[%.3f, %.3f]\n", min_x, max_x, min_y, max_y, min_z, max_z);

    fb_write_ppm("test.ppm", &fb);
    fb_free(&fb);

    return 0;
}
