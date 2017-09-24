#include "tod/exception.h"
namespace tod
{

//-----------------------------------------------------------------------------
Exception::Exception
(const char* msg, int line,
 const char* file_name, const char* func_name):
 std::runtime_error(msg)
{
    String extracted_file_name(file_name);
    extracted_file_name =
        extracted_file_name.substr(extracted_file_name.rfind("/") + 1, -1);
    
    this->line = line;
    this->desc.format("%s:\n%s(%d):\n\n%s\n",
        extracted_file_name.c_str(),
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
