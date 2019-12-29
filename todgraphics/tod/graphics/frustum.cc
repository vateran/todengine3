#include "tod/graphics/frustum.h"

#include <DirectXMath.h>

namespace tod::graphics
{

//----------------------------------------------------------------------------
void Frustum::makeFrustum(const Matrix44& vp)
{
    //https://community.khronos.org/t/extract-clip-planes-from-projection-matrix/49338
    //https://www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf

    // Left clipping plane.
    this->plane[0].set(vp.m14 + vp.m11, vp.m24 + vp.m21, vp.m34 + vp.m31, vp.m44 + vp.m41);
    // Right clipping plane.
    this->plane[1].set(vp.m14 - vp.m11, vp.m24 - vp.m21, vp.m34 - vp.m31, vp.m44 - vp.m41);
    // Top clipping plane.
    this->plane[2].set(vp.m14 - vp.m12, vp.m24 - vp.m22, vp.m34 - vp.m32, vp.m44 - vp.m42);
    // Bottom clipping plane.
    this->plane[3].set(vp.m14 + vp.m12, vp.m24 + vp.m22, vp.m34 + vp.m32, vp.m44 + vp.m42);
    // Near clipping plane.
    this->plane[4].set(vp.m13, vp.m23, vp.m33, vp.m43);
    // Far clipping plane.
    this->plane[5].set(vp.m14 - vp.m13, vp.m24 - vp.m23, vp.m34 - vp.m33, vp.m44 - vp.m43);
    
    for (uint32 i = 0; i < 6; i++)
    {
        this->plane[i].normalize();
    }
}


//----------------------------------------------------------------------------
void Frustum::makeFrustum
(float screen_depth, const Matrix44& view, const Matrix44& projection)
{
    Matrix44 p(projection);

    //절두체에서 최소 z 거리를 계산
    float z_min = -p.m(3, 2) / p.m(2, 2);
    float r = screen_depth / (screen_depth - z_min);

    //업데이트된 값을 다시 투영 행렬에 설정
    p.m(2, 2) = r;
    p.m(3, 2) = -r * z_min;

    //절두체 matrix 생성
    Matrix44 finalMatrix(view * p);

    //near plane
    {
        float x = finalMatrix.m(0, 3) + finalMatrix.m(0, 2);
        float y = finalMatrix.m(1, 3) + finalMatrix.m(1, 2);
        float z = finalMatrix.m(2, 3) + finalMatrix.m(2, 2);
        float w = finalMatrix.m(3, 3) + finalMatrix.m(3, 2);
        this->plane[0] = { x, y, z, w };
        this->plane[0].normalize();
    }

    //far plane
    {
        float x = finalMatrix.m(0, 3) - finalMatrix.m(0, 2);
        float y = finalMatrix.m(1, 3) - finalMatrix.m(1, 2);
        float z = finalMatrix.m(2, 3) - finalMatrix.m(2, 2);
        float w = finalMatrix.m(3, 3) - finalMatrix.m(3, 2);
        this->plane[1] = { x, y, z, w };
        this->plane[1].normalize();
    }

    //left plane
    {
        float x = finalMatrix.m(0, 3) + finalMatrix.m(0, 0);
        float y = finalMatrix.m(1, 3) + finalMatrix.m(1, 0);
        float z = finalMatrix.m(2, 3) + finalMatrix.m(2, 0);
        float w = finalMatrix.m(3, 3) + finalMatrix.m(3, 0);
        this->plane[2] = { x, y, z, w };
        this->plane[2].normalize();
    }

    //right plane
    {
        float x = finalMatrix.m(0, 3) - finalMatrix.m(0, 0);
        float y = finalMatrix.m(1, 3) - finalMatrix.m(1, 0);
        float z = finalMatrix.m(2, 3) - finalMatrix.m(2, 0);
        float w = finalMatrix.m(3, 3) - finalMatrix.m(3, 0);
        this->plane[3] = { x, y, z, w };
        this->plane[3].normalize();
    }

    //top plane
    {
        float x = finalMatrix.m(0, 3) - finalMatrix.m(0, 1);
        float y = finalMatrix.m(1, 3) - finalMatrix.m(1, 1);
        float z = finalMatrix.m(2, 3) - finalMatrix.m(2, 1);
        float w = finalMatrix.m(3, 3) - finalMatrix.m(3, 1);
        this->plane[4] = { x, y, z, w };
        this->plane[4].normalize();
    }

    //bottom plane
    {
        float x = finalMatrix.m(0, 3) + finalMatrix.m(0, 1);
        float y = finalMatrix.m(1, 3) + finalMatrix.m(1, 1);
        float z = finalMatrix.m(2, 3) + finalMatrix.m(2, 1);
        float w = finalMatrix.m(3, 3) + finalMatrix.m(3, 1);
        this->plane[5] = { x, y, z, w };
        this->plane[5].normalize();
    }
}


//----------------------------------------------------------------------------
bool Frustum::checkCullPoint(const Vector3& point) const
{
    for (int32 i = 0; i < 6; ++i)
    {
        if (this->plane[i].dot(point) < 0.0f)
        {
            return false;
        }
    }

    return true;
}


//----------------------------------------------------------------------------
void Frustum::checkCullAABB(const AABB& aabb, OUT Cull& result) const
{
    Vector3 corner[8];
    const Vector3& min = aabb.min;
    const Vector3& max = aabb.max;

    corner[0] = min;
    corner[1] = Vector3(min.x, max.y, min.z);
    corner[2] = Vector3(max.x, max.y, min.z);
    corner[3] = Vector3(max.x, min.y, min.z);
    corner[4] = Vector3(min.x, min.y, max.z);
    corner[5] = Vector3(min.x, max.y, max.z);
    corner[6] = max;
    corner[7] = Vector3(max.x, min.y, max.z);

    bool outside = false;
    uint32 inside = 0;
    for (uint32 i = 0; i < 6; ++i)
    {
        auto& plane = this->plane[i];
        for (uint32 j = 0; j < 8; ++j)
        {   
            auto& c = corner[j];
            float d = plane.dot(c);

            if (d <= 0)
            {   
                ++inside;
                continue;
            }
            else if (0 < d)
            {
                break;
            }
        }
    }

    if (48 == inside)
    {
        result = CULL_INSIDE;
    }
    else if (inside == 0)
    {
        result = CULL_OUTSIDE;
    }
    else
    {
        result = CULL_INTERSECT;
    }
}


//-----------------------------------------------------------------------------
void Frustum::checkCullSphere(const AABB& aabb, OUT Cull& result) const
{
    const Vector3 center = aabb.center();
    for (uint32 i = 0; i < 6; ++i)
    {
        if (this->plane[i].dot(center) + aabb.radius < 0)
        {
            result = CULL_OUTSIDE;
            return;
        }
    }

    result = CULL_INSIDE;
}

}
