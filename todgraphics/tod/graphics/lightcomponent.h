#pragma once
#include "tod/component.h"
#include "tod/graphics/light.h"
namespace tod::graphics
{

class LightComponent : public Derive<LightComponent, Component>
{
public:
    LightComponent()
    : renderer("/sys/renderer")
    {
    }
    virtual~LightComponent()
    {
    }
    
    void setIntensity(float value)
    {
        this->light.intensity = value;
    }
    float getIntensity()
    {
        return this->light.intensity;
    }
    void setColor(const Color& color)
    {
        this->light.color = color;
    }
    const Color& getColor()
    {
        return this->light.color;
    }
    void setLinearAttenuation(float value)
    {
        this->light.attenuationLinear = value;
    }
    float getLinearAttenuation()
    {
        return this->light.attenuationLinear;
    }
    void setQuadraticAttenuation(float value)
    {
        this->light.attenuationQuadratic = value;
    }
    float getQuadraticAttenuation()
    {
        return this->light.attenuationQuadratic;
    }
    Light* getLight()
    {
        return &this->light;
    }
    
    static void bindProperty()
    {
        BIND_PROPERTY(float, "intensity", "intensity", setIntensity, getIntensity, 1, PropertyAttr::DEFAULT);
        BIND_PROPERTY(const Color&, "color", "color", setColor, getColor, Color(), PropertyAttr::DEFAULT);
        BIND_PROPERTY(float, "linear_attenuation", "linear_attenuation", setLinearAttenuation, getLinearAttenuation, 1, PropertyAttr::DEFAULT);
        BIND_PROPERTY(float, "quadratic_attenuation", "quadratic_attenuation", setQuadraticAttenuation, getQuadraticAttenuation, 1, PropertyAttr::DEFAULT);
    }
    
private:
    ObjRef<Renderer> renderer;
    Light light;
};

}
