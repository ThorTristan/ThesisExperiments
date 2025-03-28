#pragma once
#include "common.h"


class RenderManager
{

public:
	RenderManager();
	~RenderManager();

	void prepareScene();
	void presentScene();

	SDL_Window* window;

	SDL_Renderer* renderer;

	TTF_Font* font;


};

