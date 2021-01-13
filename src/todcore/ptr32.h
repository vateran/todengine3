#pragma once
#include "tod/basetype.h"
#include "tod/singleton.h"
namespace tod
{

template <typename TYPE>
class Ptr32
{
public:
    typedef Ptr32<TYPE> SelfType;

public:
    Ptr32();
    Ptr32(TYPE* ptr);

    inline void reset();
    inline bool null() const;
    inline TYPE* get() const;

    inline TYPE* operator -> () const;
    inline bool operator == (TYPE* ptr) const;
    inline bool operator != (TYPE* ptr) const;

private:
    struct HighBitStorage;
    void encode(void* ptr);
    void* decode() const;

private:
    uint32 pointer;
};

}