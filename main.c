#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "vector_math.h"
#include "render.h"

#define PI 3.14159265f
#define WIDTH 1600
#define HEIGHT 900
#define NFRAMES 60

vec3 cube_vertices[8] = {
    {-1, -1, -1}, { 1, -1, -1}, { 1, 1, -1}, {-1, 1, -1}, {-1, -1, 1}, { 1, -1, 1}, { 1, 1, 1}, {-1, 1, 1}
};

int cube_edges[12][2] = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}
};

// triangle cube_triangles[12] = {
//     {4, 5, 6, {1.0f, 0.3f, 0.3f}}, {4, 6, 7, {0.3f, 0.3f, 0.3f}}, {1, 0, 3, {0.3f, 1.0f, 1.0f}}, {1, 3, 2, {0.3f, 1.0f, 1.0f}}, {5, 1, 2, {1.0f, 1.0f, 0.3f}}, {5, 2, 6, {0.3f, 1.0f, 0.3f}}, {0, 4, 7, {1.0f, 0.3f, 1.0f}}, {0, 7, 3, {1.0f, 0.3f, 1.0f}}, {7, 6, 2, {0.3f, 0.3f, 1.0f}}, {7, 2, 3, {0.3f, 0.3f, 1.0f}}, {0, 1, 5, {1.0f, 1.0f, 0.3f}}, {0, 5, 4, {1.0f, 1.0f, 0.3f}}
// };

// triangle cube_triangles[12] = {
//     {4, 5, 6, {247.0f, 37.0f, 133.0f}}, {4, 6, 7, {247.0f, 37.0f, 133.0f}}, {1, 0, 3, {181.0f, 23.0f, 158.0f}}, {1, 3, 2, {181.0f, 23.0f, 158.0f}}, {5, 1, 2, {114.0f, 9.0f, 183.0f}}, {5, 2, 6, {114.0f, 9.0f, 183.0f}}, {0, 4, 7, {86.0f, 11.0f, 173.0f}}, {0, 7, 3, {86.0f, 11.0f, 173.0f}}, {7, 6, 2, {72.0f, 12.0f, 168.0f}}, {7, 2, 3, {72.0f, 12.0f, 168.0f}}, {0, 1, 5, {58.0f, 12.0f, 163.0f}}, {0, 5, 4, {58.0f, 12.0f, 163.0f}}
// };

triangle cube_triangles[12] = {
    {4, 5, 6, {210.0f, 15.0f, 57.0f}}, {4, 6, 7, {210.0f, 15.0f, 57.0f}},
    {1, 0, 3, { 64.0f, 160.0f, 43.0f}}, {1, 3, 2, { 64.0f, 160.0f, 43.0f}},
    {5, 1, 2, { 30.0f, 102.0f, 245.0f}}, {5, 2, 6, { 30.0f, 102.0f, 245.0f}},
    {0, 4, 7, {136.0f, 57.0f, 239.0f}}, {0, 7, 3, {136.0f, 57.0f, 239.0f}},
    {7, 6, 2, {223.0f, 142.0f, 29.0f}}, {7, 2, 3, {223.0f, 142.0f, 29.0f}},
    {0, 1, 5, {254.0f, 100.0f, 11.0f}}, {0, 5, 4, {254.0f, 100.0f, 11.0f}}
};

mesh cube_mesh = {
    cube_vertices,
    cube_triangles,
    8,
    12
};

static void render_scene(frame_buffer *fb, mat4 model_a, mat4 model_b, mat4 vp, int swap) {
    fb_clear(fb, 0, 0, 0);
    if (!swap) {
        render_mesh(fb, &cube_mesh, model_a, vp);
        render_mesh(fb, &cube_mesh, model_b, vp);
    } else {
        render_mesh(fb, &cube_mesh, model_b, vp);
        render_mesh(fb, &cube_mesh, model_a, vp);
    }
}

int main(void) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return 1;
    }

    SDL_Window* window = NULL;
    SDL_Renderer* render = NULL;
    if (!SDL_CreateWindowAndRenderer("OKOKOK", WIDTH, HEIGHT, SDL_WINDOW_HIGH_PIXEL_DENSITY, &window, &render)) {
        SDL_Quit();
        return 1;
    }

    printf("density = %f\n", SDL_GetWindowPixelDensity(window));

    SDL_Texture *texture = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT);
    if (texture == NULL) {
        fprintf(stderr, "Texture failed: %s\n", SDL_GetError());
        SDL_DestroyRenderer(render);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

    // float cx = -1.0f;
    // float cy = -1.0f;
    bool running = true;

    frame_buffer fb = new_frame_buffer(WIDTH, HEIGHT);
    mat4 view = mat4_look_at(vector3(0, 0, 5), vector3(0, 0, 0), vector3(0, 1, 0));
    mat4 proj = mat4_perspective(60.0f * PI / 180.0f, (float) WIDTH / HEIGHT, 0.1f, 100.0f);

    // Uint64 start_ms = SDL_GetTicks();
    int swap_order = 0;
    int dragging = false;
    float sensitivity = 0.005f;
    mat4 rotation = mat4_identity();

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            SDL_ConvertEventToRenderCoordinates(render, &event);
            switch (event.type) {

            case SDL_EVENT_QUIT:
                running = false;
                break;
 
            case SDL_EVENT_MOUSE_MOTION:
                // cx = event.motion.x;
                // cy = event.motion.y;
                
                if (dragging) {
                    mat4 ro = mat4_mul(mat4_rotate_x(event.motion.yrel * sensitivity), mat4_rotate_y(event.motion.xrel * sensitivity));
                    rotation = mat4_mul(ro, rotation);
                }

                printf("motion x=%7.1f y=%7.1f dx=%+5.1f dy=%+5.1f\n", event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
                break;
 
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    dragging = true;
                }

                printf("button %u down at (%.1f, %.1f), clicks=%u\n", event.button.button, event.button.x, event.button.y, event.button.clicks);
                break;

            case SDL_EVENT_MOUSE_BUTTON_UP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    dragging = false;
                }

                break;
 
            case SDL_EVENT_WINDOW_MOUSE_LEAVE:
                // cx = cy = -1.0f;
                break;
 
            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_ESCAPE) {
                    running = false;
                } else if (event.key.key == SDLK_G) {
                    float gx, gy;
                    SDL_GetGlobalMouseState(&gx, &gy);
                    printf("global x=%7.1f y=%7.1f\n", gx, gy);
                } else if (event.key.key == SDLK_Z) {
                    depth_test_enabled = !depth_test_enabled;
                    fprintf(stderr, "depth test %s\n", depth_test_enabled ? "ON" : "OFF");
                } else if (event.key.key == SDLK_D) {
                    fb_write_depth_ppm("depth.ppm", &fb);
                    fprintf(stderr, "saved depth.ppm\n");
                } else if (event.key.key == SDLK_O) {
                    swap_order = !swap_order;
                    fprintf(stderr, "draw order: %s\n", swap_order ? "B then A" : "A then B");
                } else if (event.key.key == SDLK_L) {
                    light_enabled = !light_enabled;
                    fprintf(stderr, "lighting %s\n", light_enabled ? "ON" : "OFF");
                }
                break;
            }
        }

        // mat4 model = mat4_mul(mat4_rotate_x(0.005 * cy), mat4_rotate_y(cx * 0.005));
        // float angle_x = 0.005f * cx;
        // float angle_y = 0.005f * cy;

        // mat4 model_a = mat4_mul(mat4_rotate_x(angle_y), mat4_rotate_y(angle_x));
        // mat4 model_b = mat4_mul(mat4_translate(0.5f, 0.5f, 0.5f), mat4_mul(mat4_rotate_x(angle_y), mat4_rotate_y(angle_x)));
        
        mat4 model_a = rotation;
        mat4 model_b = mat4_mul(mat4_mul(mat4_translate(0.5f, 0.5f, 0.5f), mat4_mul(mat4_rotate_x(180), mat4_rotate_y(180))), rotation);

        // mat4 mvp = mat4_mul(proj, mat4_mul(view, model));
        // fb_clear(&fb, 0, 0, 0);
        // render_wireframe(&fb, mvp, cube_vertices, cube_edges);
        // render_mesh(&fb, &cube_mesh, mvp);

        // mat4 mvp_a = mat4_mul(proj, mat4_mul(view, model_a));
        // mat4 mvp_b = mat4_mul(proj, mat4_mul(view, model_b));
        mat4 vp = mat4_mul(proj, view);

        render_scene(&fb, model_a, model_b, vp, swap_order);

        SDL_UpdateTexture(texture, NULL, fb.color, WIDTH * 3);
        SDL_RenderClear(render);
        SDL_RenderTexture(render, texture, NULL, NULL);
        SDL_RenderPresent(render);
    }

    fb_free(&fb);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
