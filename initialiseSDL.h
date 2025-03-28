#pragma once
#include "common.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 500


SDL_Window* initSDLWindow();

SDL_Renderer* initRenderer(SDL_Window* window);


TTF_Font* initTTF();