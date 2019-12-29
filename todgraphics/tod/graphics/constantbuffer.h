#pragma once
#include "tod/basetype.h"
namespace tod::graphics
{

class ConstantBuffer
{
public:
    virtual~ConstantBuffer() {}

    virtual bool create(uint32 size) = 0;
    virtual void destroy() = 0;
    virtual bool isValid() = 0;

    template <typename T>
    bool lock(T** ptr);
    virtual bool unlock() = 0;

protected:
    virtual void* getDataPtr() = 0;
};


template <typename T>
bool ConstantBuffer::lock(T** ptr)
{
    *ptr = reinterpret_cast<T*>(this->getDataPtr());
    return true;
}


}