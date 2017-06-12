#include "tod/graphics/renderer.h"
namespace tod::graphics
{ 
    
void Renderer::update_transform
(Node* current, const Matrix44& parent_transform, bool parent_transform_updated)
{
    if (!current->isVisible()) return;
    
    //TransformComponent로 WorldTransform 을 업데이트
    //Scene에 있는 모든 Node들은 TransformComponent가 있다고 가정한다
    auto tc = current->findComponent<TransformComponent>();
    if (nullptr == tc) return;
    
    //transform 이 변경되었다면 업데이트
    bool transform_dirty = parent_transform_updated
    || tc->isTransformDirty();
    if (transform_dirty)
        tc->updateWorldTransform(parent_transform);
    
    
    
    auto sc = current->findComponent<ShaderComponent>();
    if (nullptr != sc) sc->begin();
    
    
    
    auto rc = current->findComponent<RenderComponent>();
    if (nullptr != rc) rc->render();
    
    
    
    //자식 노드들의 transform 업데이트
    for (auto& child : current->getChildren())
    {
        this->update_transform(
                               child,
                               tc->getWorldTransformMatrix(),
                               transform_dirty);
    }
    
    
    if (nullptr != sc) sc->end();
}
    
}
