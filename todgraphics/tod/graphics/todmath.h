#pragma once
namespace tod::graphics
{
    
class Math
{
public:
    static constexpr float PI = 3.14159265f;
    static constexpr float DEG2RAD = PI / 180.0f;
    static constexpr float RAD2DEG = 180.0f / PI;
    static constexpr float EPSILON = 0.00001f;
    
    static float abs(float value);
    static float sin(float rad);
    static float asin(float x);
    static float cos(float rad);
    static float acos(float x);
    static float tan(float rad);
    static float atan2(float x, float y);
    static inline float deg2rad(float deg);
    static inline float rad2deg(float rad);
    
public:
    static void init() {}
};
    
    
float Math::deg2rad(float deg)
{
    return deg * Math::DEG2RAD;
}
    
    
float Math::rad2deg(float rad)
{
    return rad * Math::RAD2DEG;
}
    
}
