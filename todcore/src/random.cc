#include "tod/random.h"
namespace tod
{

//-----------------------------------------------------------------------------
int Random::uniformInt(int min, int max)
{
    return this->gen() % (max - min + 1) + min;
}


//-----------------------------------------------------------------------------
float Random::uniformFloat(float min, float max)
{
    double ret = this->gen();
    return static_cast<float>((ret / this->gen.max()) * (max - min) + min);
}

}
