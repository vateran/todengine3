#pragma once
#include "tod/rect.h"
#include "tod/object.h"
#include "tod/graphics/matrix44.h"
namespace tod::graphics
{

class Camera : public Derive<Camera, Object>
{
public:
    enum
    {
        CLEAR_COLOR,
        CLEAR_DEPTH,
        CLEAR_STENCIL,
        HDR,
        
        DIRTY_PROJECTION,
        
        FLAG_MAX,
    };

public:
    Camera():
      exposure(1)
    , gamma(2.2f)
    , nearPlane(0.1f)
    , farPlane(10000)
    , fov(45)
    , width(800)
    , height(600)
    {
        this->flags[HDR] = true;
        this->flags[CLEAR_COLOR] = true;
        this->flags[CLEAR_DEPTH] = true;
        this->flags[DIRTY_PROJECTION] = true;
    }
    
    //virtual void clear()=0;
    
    inline void setHDR(bool value)
    {
        this->flags[HDR] = value;
    }
    inline bool isHDR()
    {
        return this->flags[HDR];
    }
    inline void setExposure(float value)
    {
        this->exposure = value;
    }
    inline float getExposure()
    {
        return this->exposure;
    }
    inline void setGamma(float value)
    {
        this->gamma = value;
    }
    inline float getGamma()
    {
        return this->gamma;
    }
    
    virtual void setClearColor(bool value)
    {
        this->flags[CLEAR_COLOR] = value;
    }
    inline bool isClearColor()
    {
        return this->flags[CLEAR_COLOR];
    }
    virtual void setClearDepth(bool value)
    {
        this->flags[CLEAR_DEPTH] = value;
    }
    inline bool isClearDepth()
    {
        return this->flags[CLEAR_DEPTH];
    }
    virtual void setClearStencil(bool value)
    {
        this->flags[CLEAR_STENCIL] = value;
    }
    inline bool isClearStencil()
    {
        return this->flags[CLEAR_STENCIL];
    }
    
    inline void setNear(float value)
    {
        this->nearPlane = value;
        this->flags[DIRTY_PROJECTION] = true;
    }
    inline float getNear()
    {
        return this->nearPlane;
    }
    inline void setFar(float value)
    {
        this->farPlane = value;
        this->flags[DIRTY_PROJECTION] = true;
    }
    inline float getFar()
    {
        return this->farPlane;
    }
    inline void setFov(float value)
    {
        this->fov = value;
        this->flags[DIRTY_PROJECTION] = true;
    }
    inline float getFov()
    {
        return this->fov;
    }
    inline void setWidth(float value)
    {
        this->width = value;
        this->flags[DIRTY_PROJECTION] = true;
    }
    inline float getWidth()
    {
        return this->width;
    }
    inline void setHeight(float value)
    {
        this->height = value;
        this->flags[DIRTY_PROJECTION] = true;
    }
    inline float getHeight()
    {
        return this->height;
    }
    inline float getAspectRatio()
    {
        return this->width / this->height;
    }
    const Matrix44& getProjectionMatrix()
    {
        if (this->flags[DIRTY_PROJECTION])
        {
            this->flags[DIRTY_PROJECTION] = false;
            this->projMatrix.perspectiveLH(
                  this->fov
                , this->getAspectRatio()
                , this->nearPlane
                , this->farPlane);
        }

        return this->projMatrix;
    }
    
protected:
    std::bitset<FLAG_MAX> flags;
    float exposure;
    float gamma;
    float nearPlane;
    float farPlane;
    float fov;
    float width;
    float height;
    Matrix44 projMatrix;
};

}
