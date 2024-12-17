#pragma once

#include "window.h"

#include <SDL.h>

class SDLWindow : public IWindow
{
public:
    SDLWindow(const WindowProperties &properties);
    ~SDLWindow() override;

    void OnUpdate() override;

    [[nodiscard]] uint32_t GetWidth() const override
    {
        return settings.Width;
    }

    [[nodiscard]] uint32_t GetHeight() const override
    {
        return settings.Height;
    }

    [[nodiscard]] void *GetNativeWindow() const override
    {
        return native;
    }

    bool Close = false;

private:
    WindowProperties settings;
    SDL_Window *native;
};