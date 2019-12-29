#pragma once
#include "tod/object.h"
namespace tod::graphics
{

class RenderInterface;

class Renderer : public SingletonDerive<Renderer, Object>
{
public:
    enum class RenderInterfaceType
    {
        Direct3D11,
        OpenGL,
    };

public:
    Renderer();
    ~Renderer();

    bool create(RenderInterfaceType render_interface_type);
    void destroy();

    bool isValid();

    RenderInterface* getRenderInterface();

public:
    RenderInterface* renderInterface;
};

}