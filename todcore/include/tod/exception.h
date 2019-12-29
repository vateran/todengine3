#pragma once
#include <cassert>
#include "tod/platformdef.h"
#include "tod/string.h"
#include "tod/log.h"
namespace tod
{

//!@ingroup Core
//!@brief 예외처리를 위한 클래스
class Exception : public std::exception
{
public:
    Exception(const char* msg, int32 line,
              const char* file_name, const char* func_name);
    
    const char* what() const noexcept override;
    
private:
    int32 line;
    String desc;
};

#define TOD_THROW_EXCEPTION(msg, ...) { throw tod::Exception(\
    tod::String::fromFormat(msg, __VA_ARGS__).c_str(),\
    __LINE__, __FILE__, __PRETTY_FUNCTION__); }
#define TOD_RETURN_TRACE(...) { TOD_LOG("BACKTRACE", "") return __VA_ARGS__; }
#define TOD_ASSERT(exp, msg, ...) do { assert(exp); } while (false)
    
}
