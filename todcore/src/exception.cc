#include "tod/exception.h"
namespace tod
{

//-----------------------------------------------------------------------------
Exception::Exception
(const char* msg, int line,
 const char* file_name, const char* func_name):
 std::runtime_error(msg)
{
    this->line = line;
    this->desc.format("%s:%s(%d): %s\n",
        file_name,
        func_name,
        this->line,
        this->std::runtime_error::what());
}

    
//-----------------------------------------------------------------------------
const char* Exception::what() const _NOEXCEPT
{
    return this->desc.c_str();
}
    
}
