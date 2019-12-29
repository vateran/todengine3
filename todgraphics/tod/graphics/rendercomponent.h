#pragma once
#include "tod/component.h"
#include "tod/graphics/aabb.h"
namespace tod::graphics
{

class Shader;
class RenderComponent : public Derive<RenderComponent, Component>
{
public:
    RenderComponent()
    {
        this->aabbIndex = AABBStorage::instance()->alloc();
    }
    virtual~RenderComponent()
    {
        AABBStorage::instance()->dealloc(this->aabbIndex);
    }

    virtual void render(Shader* shader, uint32 passes)=0;
    
    inline const AABB& getAABB() const { return *AABBStorage::instance()->get(this->aabbIndex); }

protected:
    void setAABB(const AABB& aabb) { *AABBStorage::instance()->get(this->aabbIndex) = aabb; }

private:
    uint32 aabbIndex;
};

}
