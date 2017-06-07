#pragma once
#include <stdexcept>
#include "tod/string.h"
namespace tod
{

//!@ingroup Core
//!@brief 예외처리를 위한 클래스
class Exception : public std::runtime_error
{
public:
    Exception(const char* msg, int line,
              const char* file_name, const char* func_name);
    
    const char* what() const _NOEXCEPT override;
    
private:
    int line;
    String desc;
};
    

#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif
#define TOD_THROW_EXCEPTION(msg, ...) do {\
throw tod::Exception(\
tod::String::fromFormat(msg, ## __VA_ARGS__).c_str(),\
__LINE__, __FILE__, __PRETTY_FUNCTION__); } while (false)
    
}
