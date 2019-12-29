#pragma once
#include "tod/component.h"
namespace tod::graphics
{

    class Shader;
    class BehaviorComponent : public Derive<BehaviorComponent, Component>
    {
    public:
        virtual void update() = 0;

    };

}
