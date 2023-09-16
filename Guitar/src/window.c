#include "window.h"
#include <string.h>

Window* createWindow(const char* name, i32 width, i32 height)
{
    Window* window = malloc(sizeof(Window));
    if (window == NULL)
        return NULL;

    window->sdlWindow = NULL;
    window->Renderer = NULL;
    window->Width = width;
    window->Height = height;

    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);

    window->sdlWindow = SDL_CreateWindow(name,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);

    if (window->sdlWindow == NULL) {
        SDL_Log("Error SDL_CreateWindow %s", SDL_GetError());
        exit(-1);
    }

    u32 flags = 0;
    flags |= SDL_RENDERER_ACCELERATED;
    flags |= SDL_RENDERER_PRESENTVSYNC;

    window->Renderer = SDL_CreateRenderer(window->sdlWindow, -1, flags);

    if (window->Renderer == NULL) {
        SDL_Log("Error SDL_CreateRenderer %s", SDL_GetError());
        exit(-1);
    }

    return window;
}

void destroyWindow(Window* window)
{
    SDL_DestroyRenderer(window->Renderer);
    SDL_DestroyWindow(window->sdlWindow);
    SDL_Quit();
    free(window);
}