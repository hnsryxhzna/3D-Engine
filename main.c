#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "vector_math.h"
#include "render.h"

#define PI 3.14159265f
#define WIDTH 512
#define HEIGHT 512
#define NFRAMES 60

vec3 cube_vertices[8] = {
    {-1, -1, -1}, { 1, -1, -1}, { 1, 1, -1}, {-1, 1, -1}, {-1, -1, 1}, { 1, -1, 1}, { 1, 1, 1}, {-1, 1, 1}
};

int cube_edges[12][2] = {
    {0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6}, {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}
};

int main(void) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return 1;
    }

    SDL_Window* window = NULL;
    SDL_Renderer* render = NULL;
    if (!SDL_CreateWindowAndRenderer("OKOKOK", 1600, 900, SDL_WINDOW_HIGH_PIXEL_DENSITY, &window, &render)) {
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

    float cx = -1.0f;
    float cy = -1.0f;
    bool running = true;

    frame_buffer fb = new_frame_buffer(WIDTH, HEIGHT);

    mat4 view = mat4_look_at(vector3(0, 0, 5), vector3(0, 0, 0), vector3(0, 1, 0));
    mat4 proj = mat4_perspective(60.0f * PI / 180.0f, (float)WIDTH / HEIGHT, 0.1f, 100.0f);

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            SDL_ConvertEventToRenderCoordinates(render, &event);
            switch (event.type) {

            case SDL_EVENT_QUIT:
                running = false;
                break;
 
            case SDL_EVENT_MOUSE_MOTION:
                cx = event.motion.x;
                cy = event.motion.y;
                printf("motion x=%7.1f y=%7.1f dx=%+5.1f dy=%+5.1f\n", event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel);
                break;
 
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                printf("button %u down at (%.1f, %.1f), clicks=%u\n", event.button.button, event.button.x, event.button.y, event.button.clicks);
                break;
 
            case SDL_EVENT_WINDOW_MOUSE_LEAVE:
                cx = cy = -1.0f;
                break;
 
            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_ESCAPE) {
                    running = false;
                } else if (event.key.key == SDLK_G) {
                    float gx, gy;
                    SDL_GetGlobalMouseState(&gx, &gy);
                    printf("global x=%7.1f y=%7.1f\n", gx, gy);
                }
                break;
            }
        }

        mat4 model = mat4_mul(mat4_rotate_x(0.6 * cx), mat4_rotate_y(cy * 0.6));
        mat4 mvp = mat4_mul(proj, mat4_mul(view, model));

        fb_clear(&fb, 30, 60, 90);
        render_wireframe(&fb, mvp, cube_vertices, cube_edges);

        SDL_UpdateTexture(texture, NULL, fb.color, WIDTH * 3);
        SDL_RenderClear(render);
        SDL_RenderTexture(render, texture, NULL, NULL);
        SDL_RenderPresent(render);
    }










    // for (int f = 0; f < NFRAMES; f++) {
    //     float angle = 2.0f * PI * f / NFRAMES;

    //     mat4 model = mat4_mul(mat4_rotate_x(0.4f), mat4_rotate_y(angle));
    //     mat4 mvp = mat4_mul(proj, mat4_mul(view, model));

    //     fb_clear(&fb, 30, 60, 90);
    //     render_wireframe(&fb, mvp, cube_vertices, cube_edges);

    //     char name[64];
    //     snprintf(name, sizeof name, "frame_%03d.ppm", f);
    //     fb_write_ppm(name, &fb);
    // }








    fb_free(&fb);
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
