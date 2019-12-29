#pragma once
#include "tod/graphics/color.h"
namespace tod::graphics
{

class Light
{
public:
    Light()
        : intensity(1)
        , specularStrength(0.5)
        , attenuationLinear(1)
        , attenuationQuadratic(1)
    {
        this->color = { 255, 255, 255, 255 };
    }

public:
    Vector3 dir;
    Color color;
    float intensity;
    float specularStrength;
    float attenuationLinear;
    float attenuationQuadratic;
};

}
