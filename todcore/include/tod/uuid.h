#pragma once
#include <memory>
#include "tod/string.h"
namespace tod
{

//-----------------------------------------------------------------------------
//!@ingroup Core
//!@brief UUID를 생성하고 관리
class Uuid
{
public:
    Uuid();
    Uuid(const char* uuid_str);
    
    void generate();
    String toString();
    
    inline bool operator == (const Uuid& rhs) const;
    inline bool operator != (const Uuid& rhs) const;
    
private:
    #pragma pack(push, 1)
    struct Data
    {
        unsigned long  data1;
        unsigned short data2;
        unsigned short data3;
        unsigned char  data4[8];
    };
    #pragma pack(pop)

    union
    {
        Data sturctData;
        unsigned char array[16];
    };
};
    
    
//-----------------------------------------------------------------------------
bool Uuid::operator == (const Uuid& rhs) const
{
    return std::memcmp(this->array, rhs.array, 16) == 0;
}


//-----------------------------------------------------------------------------
bool Uuid::operator != (const Uuid& rhs) const
{
    return !(*this == rhs);
}
    
}
