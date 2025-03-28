#include "inputManager.h"

extern bool quit;

bool InputManager::Listen()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            quit = true;
            return false;
        }
    }
}