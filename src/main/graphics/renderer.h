#pragma once

#include <string>

struct RendererProperties
{
    std::string Title = "Renderer";
    bool Debug = false;
    bool PreferIntegratedGraphics = false;
};

class IRenderer
{
public:
    virtual bool Initialize() = 0;
    virtual void OnUpdate() = 0;
    virtual ~IRenderer() = default;
};