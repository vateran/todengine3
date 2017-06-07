#include "tod/string.h"
namespace tod
{

//-----------------------------------------------------------------------------
int String::hash() const
{
    unsigned int hash = 5381;
    auto str = this->c_str();
    for(auto i = 0u; i < this->size(); ++str, ++i)
        hash = ((hash << 5) + hash) + (*str);
    return hash;
}
    
    
//-----------------------------------------------------------------------------
int String::atoi(const char* str)
{
    int val = 0;
    bool neg = false;
    if (*str == '-')
    {
        neg = true;
        ++str;
    }
    while( *str )
    {
        val = val * 10 + (*str++ - '0');
    }
    return neg?-val:val;
}
    

}
