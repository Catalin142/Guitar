#pragma once

#include <SDL.h>

#include "core.h"

typedef struct 
{
	SDL_Window* sdlWindow;
	SDL_Renderer* Renderer;
	u32 Width;
	u32 Height;

} Window;

Window* createWindow(const char* name, i32 width, i32 height);
void destroyWindow(Window* window);
