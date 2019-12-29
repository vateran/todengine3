#pragma once
#include "tod/object.h"
#include "tod/graphics/graphicsdef.h"
namespace tod::graphics
{

class Texture;

class RenderTarget : public Derive<RenderTarget, Object>
{
public:
    RenderTarget()
    {
        
    }
    
    virtual bool create(int32 width, int32 height)
    {
        TOD_RETURN_TRACE(false);
    }
    
    virtual Texture* addColorTarget(Format format)
    {
        return nullptr;
    }
    virtual Texture* makeDepthTarget(Format format)
    {
        return nullptr;
    }
    virtual void blitColorTarget(RenderTarget* to_rt, int32 from_index, int32 to_index)
    {
    }
    virtual Texture* getColorTexture(int32 index)
    {
        return nullptr;
    }
    virtual Texture* getDepthTexture()
    {
        return nullptr;
    }
    
    virtual void destroy()
    {
    }
    
    virtual void begin(bool clear_color, bool clear_depth)
    {
    }
    
    
    virtual void end()
    {
    }
    
    virtual void use()
    {
    }
    
    const String& getName() const { return this->name; }
    
protected:
    String name;
    
};

}
