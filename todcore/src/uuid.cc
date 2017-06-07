#include "tod/uuid.h"
#ifdef WIN32
#include <Rpc.h>
#else
#include <uuid/uuid.h>
#endif
namespace tod
{

//-----------------------------------------------------------------------------
Uuid::Uuid()
{
    memset(this->array, 0, sizeof(this->array));
}
    
    
//-----------------------------------------------------------------------------
Uuid::Uuid(const char* uuid_str)
{
    #ifdef WIN32
    UuidFromStringA((RPC_CSTR)(uuid_str), (UUID*)this->array);
    #else
    uuid_parse(uuid_str, this->array);
    #endif
}

    
//-----------------------------------------------------------------------------
void Uuid::generate()
{
    #ifdef WIN32
    UuidCreate((UUID*)this->array);
    #else
    uuid_generate_random(this->array);
    #endif
}
    

//-----------------------------------------------------------------------------
String Uuid::toString()
{
    String ret;
    
    #ifdef WIN32
    unsigned char* str;
    UuidToStringA((UUID*)this->array, &str);
    ret.assign(reinterpret_cast<char*>(str));
    RpcStringFreeA(&str);
    #else
    char str[37];
    uuid_unparse(this->array, str);
    ret.assign(str);
    #endif
    
    return ret;
}
    
}

