#pragma once
namespace tod
{

template <typename T>
class RectBase
{
public:
    T x, y, w, h;
};
typedef RectBase<float> Rect;

}
