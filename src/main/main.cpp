#include "main.h"

#include <iostream>
using std::cout;
using std::endl;
#include <fstream>
#include <string>

#define SDL_MAIN_HANDLED true
#include <SDL.h>

#include "core/log.h"
#include "platform/sdl_window.h"
#include "graphics/vulkan_renderer.h"

int main(int argc, char *argv[])
{

    RendererProperties rProperties = {};
    rProperties.Title = "Vanadium Test Renderer";
    rProperties.Debug = true;
    rProperties.PreferIntegratedGraphics = false;
    VulkanRenderer renderer(rProperties);

    if (!renderer.Initialize())
    {
        Log::Error("Failed initializing renderer!");
        return EXIT_FAILURE;
    }

    WindowProperties wProperties = {};
    wProperties.Title = "Vanadium Test Window";
    SDLWindow window(wProperties);

    while (!window.Close)
    {
        window.OnUpdate();
    }

    return EXIT_SUCCESS;
}
