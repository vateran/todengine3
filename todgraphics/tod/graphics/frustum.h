#pragma once
#include "tod/graphics/matrix44.h"
#include "tod/graphics/aabb.h"
#include "tod/graphics/plane.h"
namespace tod::graphics
{

class Frustum
{
public:
    enum Cull
    {
        CULL_INSIDE,
        CULL_OUTSIDE,
        CULL_INTERSECT,
    };

public:
    void makeFrustum(const Matrix44& vp);
    void makeFrustum(float screen_depth, const Matrix44& view, const Matrix44& projection);

    bool checkCullPoint(const Vector3& point) const;
    void checkCullAABB(const AABB& aabb, OUT Cull& result) const;
    void checkCullSphere(const AABB& aabb, OUT Cull& result) const;

private:
    Plane plane[6];
};

}