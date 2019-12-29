#pragma once
#include <algorithm>
#include "tod/singleton.h"
#include "tod/indexedpool.h"
#include "tod/graphics/vector3.h"
#include "tod/graphics/matrix44.h"
namespace tod::graphics
{

class AABB
{
public:
    inline void clear()
    {
        this->min.clear();
        this->max.clear();
    }

    inline void set(const Vector3& min, const Vector3& max)
    {
        this->min = min;
        this->max = max;

        this->radius = (this->max - this->min).length() * 0.5f;
    }

    inline Vector3 center() const
    {
        return (this->max + this->min) * 0.5f;
    }

    void transform(const Matrix44& world)
    {
        const Vector3 translation = world.getTranslation();
        this->min += translation;
        this->max += translation;
    }

    void unionAABB(const AABB& other)
    {
        for (uint32 i = 0; i < 3; ++i)
        {
            this->min.array[i] = std::min<float>(this->min.array[i], other.min.array[i]);
            this->max.array[i] = std::max<float>(this->max.array[i], other.max.array[i]);
        }

        this->radius = (this->max - this->min).length() * 0.5f;
    }

public:
    Vector3 min;
    Vector3 max;
    float radius;
};

class AABBStorage : public Singleton<AABBStorage>
{
public:
    inline uint32 alloc() { return this->data.alloc(); }
    inline void dealloc(uint32 index) { this->data.dealloc(index); }
    inline void deallocAll() { this->data.deallocAll(); }
    inline AABB* get(uint32 index) { return this->data.get(index); }

private:
    IndexedPool<AABB> data;

};

}