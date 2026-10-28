#include <stdio.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

int main() {

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return 1;
    }

    SDL_Window* window = NULL;
    SDL_Renderer* render = NULL;

    if (!SDL_CreateWindowAndRenderer("Pointer Interaction", 2000, 500, SDL_WINDOW_HIGH_PIXEL_DENSITY, &window, &render)) {
        SDL_Quit();
        return 1;
    }

    printf("density = %.1f\n", SDL_GetWindowPixelDensity(window));

    SDL_SetRenderVSync(render, 1);

    float cx = -1.0f;
    float cy = -1.0f;
    bool running = true;

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

        SDL_RenderClear(render);
        SDL_RenderPresent(render);
    }

    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
