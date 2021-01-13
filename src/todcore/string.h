#pragma once
#include <string>
#include <algorithm>
#include <cctype>
#include <memory>
#include <bitset>
#include "tod/basetype.h"
namespace tod
{

class StaticString;

//-----------------------------------------------------------------------------
#define S(s) s
typedef std::string BaseString;

class WString : public std::wstring
{
public:
};
    
//!@ingroup Core
//!@brief TodEngine에서 사용하는 문자열을 객체
class String : public BaseString
{
public:
    String() {}
    String(const char* str):BaseString(str) {}
    String(const BaseString& str):BaseString(str) {}
    String(const String& str, size_t s):BaseString(str, s) {}
    String(const StaticString& str);
    String(String::iterator b, String::iterator e):BaseString(b, e) {}
    String(String::const_iterator b, String::const_iterator e):BaseString(b, e) {}
    String(const String& str, size_t b, size_t e):BaseString(str, b, e) {}
    
    template <typename ... ARGS>
    void format(const char* format, ARGS ... args);
    template <typename OUT_T>
    void split(const char* delims, OUT_T& ret, bool drop_empty=true) const;
    inline void ltrim();
    inline void rtrim();
    inline void trim();
    void replace(const String& from, const String& to);
    int32 hash() const;
    String extractFileExtension() const;
    String extractPath() const;
    String extractFileName() const;
    String normalizePath() const;
    String& lower();
    String& upper();
    WString toUTF8() const;
    
    template<typename ... ARGS>
    static String fromFormat(const char* format, ARGS ... args);
    static size_t findString(const char* str, const char* find_str);
    static int32 atoi(const char* str);
    static inline int64 atoi64(const char* str);
    static inline uint64 atoui64(const char* str);
    //@brief 입력된 str 의 hash 를 구한다. len이 입력되지 않으면 \0인곳까지
    static int32 hash(const char* str, size_t len=0);
    template <typename T>
    static T atof(const char* str);
};
    
}

namespace std
{
    template <>
    struct hash<tod::String>
    {
        inline std::size_t operator() (const tod::String& str) const
        {
            return str.hash();
        }
    };
}

#include "tod/string.inl"