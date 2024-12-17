#pragma once

#include <cstdint> // cross platform integer types
#include <string>

struct WindowProperties
{
    std::string Title = "Vanadium";
    uint32_t Width = 1280;
    uint32_t Height = 720;
    uint32_t X = 100;
    uint32_t Y = 100;
};

class IWindow
{
public:
    virtual void OnUpdate() = 0;
    virtual ~IWindow() = default;
    [[nodiscard]] virtual uint32_t GetWidth() const = 0;
    [[nodiscard]] virtual uint32_t GetHeight() const = 0;
    [[nodiscard]] virtual void *GetNativeWindow() const = 0;
};