#include "common.h"
#include "initialiseSDL.h"
#include "renderManager.h"

extern RenderManager renderManager;



RenderManager::RenderManager()
{

	window = nullptr;
	renderer = nullptr;
	font = nullptr;

	window = initSDLWindow();
	renderer = initRenderer(window);
	font = initTTF();




	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

	SDL_ShowCursor(0);


	std::cout << "SDL Initialised Successfuly" << std::endl;




}


RenderManager::~RenderManager()
{

	if (renderer)
	{
		SDL_DestroyRenderer(renderer);
	}

	if (window)
	{
		SDL_DestroyWindow(window);
	}

	IMG_Quit();
	SDL_Quit();

	TTF_CloseFont(font);
	TTF_Quit();


	std::cout << "SDL Cleaned up" << std::endl;
}


void RenderManager::prepareScene()
{
	//SDL_SetRenderDrawColor(renderer, 96, 128, 255, 255);
	SDL_SetRenderDrawColor(renderer, 83, 83, 83, 255);
	SDL_RenderClear(renderer);



}


void RenderManager::presentScene()
{

	SDL_RenderPresent(renderer);


}

