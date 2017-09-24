#include "tod/graphics/transformcomponent.h"
namespace tod::graphics
{

void TransformComponent::bindProperty()
{
    BIND_PROPERTY(const Vector3&, "position", "Position", setPosition, getPosition, Vector3(), PropertyAttr::DEFAULT);
    BIND_PROPERTY(const Vector3&, "scaling", "Scaling", setScaling, getScaling, Vector3(1, 1, 1), PropertyAttr::DEFAULT);
    BIND_PROPERTY(const Vector3&, "rotation", "Rotation", setRotation, getRotation, Vector3(), PropertyAttr::DEFAULT);
}


}
