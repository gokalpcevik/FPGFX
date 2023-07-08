#include <iostream>
#include <SDL2/SDL.h>
#include "App.h"

int main(int argc, char **argv)
{
    SDL_SetMainReady();

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cout << SDL_GetError() << std::endl;
    }
    return fpgfx::Start();
}