#include "sdl_window.h"

#include "core/log.h"

SDLWindow::SDLWindow(const WindowProperties &properties) : settings(properties)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        Log::Error(SDL_GetError());
        SDL_assert(false);
    }

    native = SDL_CreateWindow(properties.Title.c_str(), properties.X, properties.Y,
                              properties.Width, properties.Height, SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

    if (!native)
    {
        SDL_assert(false);
        SDL_Quit();
    }

    Log::System("SDL Window Init");
}

SDLWindow::~SDLWindow()
{
    SDL_DestroyWindow(native);
    SDL_Quit();
}

void SDLWindow::OnUpdate()
{
    SDL_Event e;

    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
        {
            Log::System("Window Quit");
            Close = true;
        }
    }
}