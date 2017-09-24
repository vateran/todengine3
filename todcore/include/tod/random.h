#pragma once
#include <random>
namespace tod
{

class Random
{
public:
    ///@return min <= value <= max
    int uniformInt(int min, int max);
    ///@return min <= value <= max
    float uniformFloat(float min, float max);
    
private:
    std::mt19937 gen;
};

}
