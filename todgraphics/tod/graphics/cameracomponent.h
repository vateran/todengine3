#pragma once
#include "tod/component.h"
#include "tod/graphics/camera.h"
namespace tod::graphics
{

class CameraComponent : public Derive<CameraComponent, Component>
{
public:
    CameraComponent():
    camera(nullptr)
    , renderer("/sys/renderer")
    , brightThreshold(2.0f)
    {
        this->camera = this->renderer->createCamera();
    }
    virtual~CameraComponent()
    {
    }
    
    inline void setHDR(bool value)
    {
        this->camera->setHDR(value);
    }
    inline bool isHDR()
    {
        return this->camera->isHDR();
    }
    inline void setExposure(float value)
    {
        this->camera->setExposure(value);
    }
    inline float getExposure()
    {
        return this->camera->getExposure();
    }
    inline void setGamma(float value)
    {
        this->camera->setGamma(value);
    }
    inline float getGamma()
    {
        return this->camera->getGamma();
    }
    
    inline void setClearColor(bool value)
    {
        this->camera->setClearColor(value);
    }
    inline bool isClearColor()
    {
        return this->camera->isClearColor();
    }
    inline void setClearDepth(bool value)
    {
        this->camera->setClearDepth(value);
    }
    inline bool isClearDepth()
    {
        return this->camera->isClearDepth();
    }
    inline void setClearStencil(bool value)
    {
        this->camera->setClearStencil(value);
    }
    inline bool isClearStencil()
    {
        return this->camera->isClearStencil();
    }
    
    inline void clear()
    {
        this->camera->clear();
    }
    
    inline void setNear(float value)
    {
        this->camera->setNear(value);
    }
    inline float getNear()
    {
        return this->camera->getNear();
    }
    inline void setFar(float value)
    {
        this->camera->setFar(value);
    }
    inline float getFar()
    {
        return this->camera->getFar();
    }
    inline void setFov(float value)
    {
        this->camera->setFov(value);
    }
    inline float getFov()
    {
        return this->camera->getFov();
    }
    inline void setWidth(float value)
    {
        this->camera->setWidth(value);
    }
    inline float getWidth()
    {
        return this->camera->getWidth();
    }
    inline void setHeight(float value)
    {
        this->camera->setHeight(value);
    }
    inline float getHeight()
    {
        return this->camera->getHeight();
    }
    inline const Matrix44& getProjectionMatrix()
    {
        return this->camera->getProjectionMatrix();
    }
    inline void setBrightThreshold(float value)
    {
        this->brightThreshold = value;
    }
    inline float getBrightThreshold()
    {
        return this->brightThreshold;
    }
    inline float getAspectRatio()
    {
        return this->camera->getAspectRatio();
    }
    inline void setDOF(const Vector4& dof)
    {
        this->dof = dof;
    }
    inline const Vector4& getDOF()
    {
        return this->dof;
    }
    
    static void bindProperty()
    {
        BIND_PROPERTY(bool, "HDR", "HDR", setHDR, isHDR, true, PropertyAttr::DEFAULT);
        BIND_PROPERTY(float, "exposure", "exposure", setExposure, getExposure, 1, PropertyAttr::DEFAULT);
        BIND_PROPERTY(float, "gamma", "gamma", setGamma, getGamma, 2.2f, PropertyAttr::DEFAULT);
        BIND_PROPERTY(float, "bright_threshold", "bright_threshold", setBrightThreshold, getBrightThreshold, 2.0f, PropertyAttr::DEFAULT);
        BIND_PROPERTY(bool, "clear_color", "clear_color", setClearColor, isClearColor, true, PropertyAttr::DEFAULT);
        BIND_PROPERTY(bool, "clear_depth", "clear_depth", setClearDepth, isClearDepth, true, PropertyAttr::DEFAULT);
        BIND_PROPERTY(bool, "clear_stencil", "clear_stencil", setClearStencil, isClearStencil, false, PropertyAttr::DEFAULT);
        BIND_PROPERTY(float, "near", "near", setNear, getNear, -10, PropertyAttr::DEFAULT);
        BIND_PROPERTY(float, "far", "far", setFar, getFar, 10000, PropertyAttr::DEFAULT);
        BIND_PROPERTY(float, "fov", "Field of View", setFov, getFov, 45.0f, PropertyAttr::DEFAULT);
        BIND_PROPERTY(float, "width", "Screen Width", setWidth, getWidth, 0.0f, PropertyAttr::DEFAULT);
        BIND_PROPERTY(float, "height", "Screen Height", setHeight, getHeight, 0.0f, PropertyAttr::DEFAULT);
        BIND_PROPERTY(const Vector4&, "DOF", "DOF", setDOF, getDOF, Vector4(), PropertyAttr::DEFAULT);
    }

private:
    ObjRef<Camera> camera;
    ObjRef<Renderer> renderer;
    float brightThreshold;
    Vector4 dof;
};

}
