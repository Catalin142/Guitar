#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <Windows.h>

#include <SDL_audio.h>
#include <SDL_timer.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL.h>

#include "core.h"
#include "window.h"
#include "input.h"
#include "guitar.h"
#include "tabulature.h"

#define GUITAR_OFFSET_Y 150

void guitarCallback(void* userdata, Uint8* stream, int len) {
    float* fstream = (float*)stream;
    Guitar* guitar = (Guitar*)userdata;

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        float output = 0.0f;
        for (int currentString = 0; currentString < NUMBER_OF_STRINGS; currentString++)
        {
            GuitarString* string = &guitar->strings[currentString];
            if (string->isRinging)
                output += lowPassFilterNext(string);
        }

        fstream[2 * i + 0] = output * guitar->masterVolume;
        fstream[2 * i + 1] = output * guitar->masterVolume;
    }
}

internal
u32 getPluckedString(const Guitar* guitar, ivec2 mousePosition)
{
    for (u32 i = 0; i < NUMBER_OF_STRINGS; i++)
        if (mousePosition.y > GUITAR_OFFSET_Y + guitar->stringSection[i].x && mousePosition.y < GUITAR_OFFSET_Y + guitar->stringSection[i].y)
            return i;
    return NUMBER_OF_STRINGS;
}

internal 
u32 getFret(const Guitar* guitar, ivec2 mousePosition)
{
    for (u32 i = 1; i <= NUMBER_OF_FRETS; i++)
        if (mousePosition.x > (i32)guitar->fretPosition[i - 1] && mousePosition.x < (i32)guitar->fretPosition[i])
            return i;

    return 0;
}

internal
i8 loadTexture(SDL_Renderer* renderer, char* filepath, SDL_Texture** tex)
{
    SDL_Surface* surface = IMG_Load(filepath);
    *tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (*tex == NULL)
        return false;
    
    return true;
}

internal
void renderStringTab(Tabulature* tab, Window* window, TTF_Font* font, SDL_Color color)
{
    SDL_Surface* text;
    SDL_Texture* text_texture;

    u32 startY = 470;
    for (u32 i = 0; i < NUMBER_OF_STRINGS; i++)
    {
        StringSection section = tab->stringFrets[i];
        u8* frets = tab->stringFrets[i].frets;
        char fretText[MAX_NUMBER_OF_SECTIONS + 1] = { '\0' };
        fretText[0] = getStringByNumber(i);
        u32 currentTextIndex = 1;
        for (u32 currentSection = 0; currentSection < section.numberOfSections; currentSection++)
        {
            if (frets[currentSection] == NO_SOUND_TAB)
            {
                fretText[currentTextIndex++] = '-';
                continue;
            }

            if (frets[currentSection] >= 10)
            {
                fretText[currentTextIndex - 1] = frets[currentSection] / 10 + '0';
                fretText[currentTextIndex++] = frets[currentSection] % 10 + '0';
            }
            else  fretText[currentTextIndex++] = frets[currentSection] + '0';
        }

        text = TTF_RenderText_Solid(font, fretText, color);
        if (!text) {
            printf("Failed to render text: %s\n", TTF_GetError());
        }

        text_texture = SDL_CreateTextureFromSurface(window->Renderer, text);

        SDL_Rect dest = { 20, startY, text->w, text->h };
        SDL_Rect src = { 0, 0, 1, 1 };

        SDL_RenderCopy(window->Renderer, text_texture, NULL, &dest);
        SDL_FreeSurface(text);
        SDL_DestroyTexture(text_texture);

        startY -= 30;
    }
}

int main(int argc, char** agrv)
{
    u8 isRunning = true;
    Input input;

    srand((u32)time(NULL));

    Guitar guitar = createGuitar();
    guitar.masterVolume = 0.3f;

    SDL_Texture* fretboardTexture;
    SDL_Texture* backgroundTexture;

    Window* window = createWindow("Guitar", 1600, 900);
    SDL_AudioSpec spec;
    spec.format     = AUDIO_F32;
    spec.channels   = 2;
    spec.freq       = SAMPLE_RATE;
    spec.samples    = BUFFER_SIZE;
    spec.userdata   = &guitar;
    spec.callback   = guitarCallback;

    if (SDL_OpenAudio(&spec, NULL) < 0) {
        printf("Failed to open Audio Device: %s\n", SDL_GetError());
        return 1;
    }

    if (TTF_Init() < 0) {
        printf("Failed to initialize SDL TTF: %s\n", TTF_GetError());
        return 1;
    }

    TTF_Font* font;

    font = TTF_OpenFont("res/VeraMono.ttf", 18);
    if (!font) {
        printf("Failed to load TTF Font: %s\n", TTF_GetError());
    }

    SDL_PauseAudio(0);

    if (loadTexture(window->Renderer, "res/fretboard.png", &fretboardTexture) == false)
        return 1;

    if (loadTexture(window->Renderer, "res/bg.png", &backgroundTexture) == false)
        return 1;

    scaleFretsToResolution(&guitar, window->Width);

    SDL_Color color = { 255, 255, 255 };

    Tabulature tab = loadTabs("res/mr_brightside.txt");

    u32 newTime = SDL_GetTicks();
    u32 oldTime = newTime;
    f32 deltaTime = 0.0f;

    guitar.technique = PICKING;

    while (isRunning) 
    {
        SDL_SetRenderDrawColor(window->Renderer, 0, 0, 0, 255);
        SDL_RenderClear(window->Renderer);

        newTime = SDL_GetTicks();
        deltaTime = (f32)(newTime - oldTime) / 1000.0f;
        oldTime = newTime;

        SDL_Rect bg_rect = {
            .x = 0,
            .y = 0,
            .w = window->Width,
            .h = window->Height
        };

        SDL_RenderCopy(window->Renderer, backgroundTexture, NULL, &bg_rect);
        SDL_Rect tex_rect = {
            .x = 0,
            .y = GUITAR_OFFSET_Y,
            .w = window->Width,
            .h = guitar.fretboardHeightInPixels
        };
        renderStringTab(&tab, window, font, color);
        SDL_RenderCopy(window->Renderer, fretboardTexture, NULL, &tex_rect);

        i32 startY = 20;
        SDL_SetRenderDrawColor(window->Renderer, 255, 255, 255, 255);
        for (int string = NUMBER_OF_STRINGS - 1; string >= 0; string--)
        {
            if (!guitar.strings[string].isRinging || guitar.strings[string].wavetable[0] == guitar.randomTable[0])
            {
                startY += 20;
                continue;
            }
            for (u32 i = 0; i < guitar.strings[string].length; i++)
            {
                SDL_Rect r;
                r.x = 10 + i * 3;
                r.y = startY + (i32)(guitar.strings[string].wavetable[i] * 50.0f);
                r.h = 3;
                r.w = 3;

                SDL_RenderFillRect(window->Renderer, &r);
            }
            startY += 20;
        }

        handleEvents(&input);
        isRunning = !input.closeEvent;
        
            playTab(&guitar, &tab, deltaTime);

        for (int i = 0; i < NUMBER_OF_STRINGS; i++)
        {
            if (pressed(input, i))
                pluckString(&guitar, i, 0);

            if (released(input, i))
                muteString(&guitar, i);
        }

        if (pressed(input, LMB))
        {
            u32 fret    = getFret(&guitar, input.mousePosition);
            u32 string  = getPluckedString(&guitar, input.mousePosition);
            if (fret != 0 && string != NUMBER_OF_STRINGS)
                pluckString(&guitar, string, fret);
        }

        if (pressed(input, RMB))
        {

            u32 string = getPluckedString(&guitar, input.mousePosition);
            if (string != NUMBER_OF_STRINGS)
                pluckString(&guitar, string, 0);
        }
        updateGuitar(&guitar);

        SDL_RenderPresent(window->Renderer);
    }

    TTF_CloseFont(font);
    TTF_Quit();

    SDL_DestroyTexture(fretboardTexture);

    destroyWindow(window);
    destroyGuitar(guitar);

    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtDumpMemoryLeaks();

    return 0;
}
