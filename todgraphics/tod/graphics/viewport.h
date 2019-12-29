#pragma once
#include "tod/basetype.h"
#include "tod/graphics/colorf.h"
namespace tod
{

class Window;

}

namespace tod::graphics
{

class Viewport
{
public:
    virtual~Viewport() {}

    virtual bool create(Window* window, uint32 multi_sample = 1) = 0;
    virtual void destroy() = 0;

    virtual void use() = 0;
    virtual void clear(const ColorF& clear_color) = 0;
    virtual void swap() = 0;

    virtual bool resize(uint32 width, uint32 height) = 0;

    virtual void setMultiSample(uint32 multi_sample) = 0;
    virtual uint32 getMultiSample() const = 0;
    virtual uint32 getWidth() const = 0;
    virtual uint32 getHeight() const = 0;
};

}