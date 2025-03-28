#include "initialiseSDL.h"

SDL_Window* initSDLWindow()
{
	int windowFlags;

	windowFlags = 0;



	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Couldn't initialize SDL: %s\n", SDL_GetError());
		return nullptr;


	}



	return SDL_CreateWindow("Grid", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags);
}


SDL_Renderer* initRenderer(SDL_Window* window)
{
	int rendererFlags;

	rendererFlags = SDL_RENDERER_ACCELERATED;


	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	return SDL_CreateRenderer(window, -1, rendererFlags);

}



TTF_Font* initTTF()
{

	if (TTF_Init() == -1)
	{
		std::cerr << "Failed to initialize SDL_ttf: " << TTF_GetError() << std::endl;
	}

	TTF_Font* font = TTF_OpenFont("Assets/Fonts/Roboto/Roboto-VariableFont_wdth,wght.ttf", 24);

	if (!font)
	{
		std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
	}

	return font;


}