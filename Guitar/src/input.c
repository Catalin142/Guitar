#include "input.h"

void setState(u8* keys, u32 key, u8 value)
{
    *keys = *keys & ~(1 << key);
    *keys = *keys | (value << key);
}

u8 isPressed(u8 keys, u32 key)
{
    return keys & (1 << key);
}

void handleKeys(u32 key, u8* keys, u8 value)
{
    switch (key) {
    case SDLK_1:
        setState(keys, NUM_1, value); break;

    case SDLK_2:
        setState(keys, NUM_2, value); break;

    case SDLK_3:
        setState(keys, NUM_3, value); break;

    case SDLK_4:
        setState(keys, NUM_4, value); break;

    case SDLK_5:
        setState(keys, NUM_5, value); break;

    case SDLK_6:
        setState(keys, NUM_6, value); break;
    }
}

u8 pressed(Input input, u32 key)
{
    return (!isPressed(input.old_keys, key) && isPressed(input.keys, key));
}

u8 released(Input input, u32 key)
{
    return (isPressed(input.old_keys, key) && !isPressed(input.keys, key));
}

void handleEvents(Input* input)
{
    input->closeEvent = false;
    input->old_keys = input->keys;

    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        switch (ev.type) {
        case SDL_QUIT:
            input->closeEvent = true; break;

        case SDL_KEYDOWN:
            handleKeys(ev.key.keysym.sym, &input->keys, true);
            break;

        case SDL_KEYUP:
            handleKeys(ev.key.keysym.sym, &input->keys, false);
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (ev.button.button == SDL_BUTTON_LEFT)
                setState(&input->keys, LMB, true);
            else if (ev.button.button == SDL_BUTTON_RIGHT)
                setState(&input->keys, RMB, true);

            break;

        case SDL_MOUSEBUTTONUP:
            if (ev.button.button == SDL_BUTTON_LEFT)
                setState(&input->keys, LMB, false);
            else if (ev.button.button == SDL_BUTTON_RIGHT)
                setState(&input->keys, RMB, false);

            break;

        case SDL_MOUSEMOTION:
            SDL_GetMouseState(&input->mousePosition.x, &input->mousePosition.y);

            break;
        }
    }
}