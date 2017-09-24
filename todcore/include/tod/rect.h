#pragma once
namespace tod::graphics
{

template <typename T>
class RectBase
{
public:
    T x, y, w, h;
};
typedef RectBase<float> Rect;

}
