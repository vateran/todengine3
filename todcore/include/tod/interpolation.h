#pragma once
namespace tod
{

template <typename T>
class InterpolationBase
{
public:
    inline static T linear(T ratio, T start, T end)
    {
        if (ratio >= 1) return end;
        return ratio * (end - start) + start;
    }
};

typedef InterpolationBase<float> Interpolation;

}
