#pragma once
#include "tod/graphics/camera.h"
namespace tod::graphics
{

class OpenGlCamera : public Derive<OpenGlCamera, Camera>
{
public:
    OpenGlCamera():clearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)
    {
    }
    
    void clear() override
    {
        OpenGl::funcs->glClear(this->clearMask);
    }
    void setClearColor(bool value) override
    {
        if (value)
            this->clearMask |= GL_COLOR_BUFFER_BIT;
        else
            this->clearMask &= ~GL_COLOR_BUFFER_BIT;
        
        BaseType::setClearColor(value);
    }
    void setClearDepth(bool value) override
    {
        if (value)
            this->clearMask |= GL_DEPTH_BUFFER_BIT;
        else
            this->clearMask &= ~GL_DEPTH_BUFFER_BIT;
    
        this->flags[CLEAR_DEPTH] = value;
    }
    void setClearStencil(bool value) override
    {
        if (value)
            this->clearMask |= GL_STENCIL_BUFFER_BIT;
        else
            this->clearMask &= ~GL_STENCIL_BUFFER_BIT;
    
        this->flags[CLEAR_STENCIL] = value;
    }
    
private:
    GLbitfield clearMask;
};

}
