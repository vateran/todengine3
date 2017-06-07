#pragma once
#include <string>
#include <bitset>
#include <functional>
#include <iostream>
#include <memory>
#include <stdlib.h>
#include "tod/platformdef.h"
namespace tod
{

//-----------------------------------------------------------------------------
#define S(s) s
typedef std::string BaseString;
    
    
//!@ingroup Core
//!@brief TodEngine에서 사용하는 문자열을 객체
class String : public BaseString
{
public:
    String() {}
    String(const char* str):BaseString(str) {}
    String(const BaseString& str):BaseString(str) {}
    String(const String& str, size_t s):BaseString(str, s) {}
    String(String::iterator b, String::iterator e):BaseString(b, e) {}
    String(String::const_iterator b, String::const_iterator e):BaseString(b, e) {}
    
    template<typename ... ARGS>
    void format(const char* format, ARGS ... args);
    template <typename OUT_T>
    void split(const char* delims, OUT_T& ret, bool drop_empty=true) const;
    inline void ltrim();
    inline void rtrim();
    inline void trim();
    int hash() const;
    
    template<typename ... ARGS>
    static String fromFormat(const char* format, ARGS ... args);
    static int atoi(const char* str);
    static inline int64 atoi64(const char* str);
    static inline uint64 atoui64(const char* str);
    template <typename T>
    static T atof(const char* str);
};
    

//-----------------------------------------------------------------------------
template<typename ... ARGS>
void String::format(const char* format, ARGS ... args)
{
    size_t size = std::snprintf(nullptr, 0, format, args ...) + 1;
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format, args ...);
    this->assign(buf.get(), buf.get() + size - 1);
}


//-----------------------------------------------------------------------------
template <typename OUT_T>
void String::split(const char* delims, OUT_T& ret, bool drop_empty) const
{
    OUT_T output;
    
    std::bitset<255> delims_bit;
    while (*delims)
    {
        unsigned char code = *delims++;
        delims_bit[code] = true;
    }
    
    String::const_iterator beg;
    bool in_token = false;
    for (auto it = this->begin(), end = this->end();
         it != end; ++it)
    {
        if (delims_bit[*it])
        {
            if (in_token)
            {
                output.push_back(typename OUT_T::value_type(beg, it));
                in_token = false;
            }
            else if (!drop_empty)
            {
                auto back_it = it;
                --back_it;
                if (back_it != this->begin())
                {
                    if (delims_bit[*back_it])
                        output.push_back(S(""));
                }
            }
        }
        else if (!in_token)
        {
            beg = it;
            in_token = true;
        }
    }
    if (in_token)
        output.push_back(
                         typename OUT_T::value_type(beg, this->end()));
    output.swap(ret);
}

    
//-----------------------------------------------------------------------------
void String::ltrim()
{
    this->erase(this->begin(), std::find_if(this->begin(), this->end(),
        std::not1(std::ptr_fun<int, int>(std::isspace))));
}


//-----------------------------------------------------------------------------
void String::rtrim()
{
    this->erase(std::find_if(this->rbegin(), this->rend(),
        std::not1(std::ptr_fun<int, int>(std::isspace))).base(), this->end());
}
    
    
//-----------------------------------------------------------------------------
void String::trim()
{
    this->ltrim();
    this->rtrim();
}

//-----------------------------------------------------------------------------
template <typename T>
T String::atof(const char* str)
{
    #define white_space(c) ((c) == ' ' || (c) == '\t')
    #define valid_digit(c) ((c) >= '0' && (c) <= '9')
    
    int frac;
    T sign, value, scale;
    
    // Skip leading white space, if any.
    
    while (white_space(*str) ) {
        str += 1;
    }
    
    // Get sign, if any.
    
    sign = 1.0;
    if (*str == '-') {
        sign = -1.0;
        str += 1;
        
    } else if (*str == '+') {
        ++str;
    }
    
    // Get digits before decimal point or exponent, if any.
    
    for (value = 0.0; valid_digit(*str); ++str) {
        value = value * 10.0 + (*str - '0');
    }
    
    // Get digits after decimal point, if any.
    
    if (*str == '.') {
        T pow10 = 10.0;
        ++str;
        while (valid_digit(*str)) {
            value += (*str - '0') / pow10;
            pow10 *= 10.0;
            ++str;
        }
    }
    
    // Handle exponent, if any.
    
    frac = 0;
    scale = 1.0;
    if ((*str == 'e') || (*str == 'E')) {
        unsigned int expon;
        
        // Get sign of exponent, if any.
        
        ++str;
        if (*str == '-') {
            frac = 1;
            ++str;
            
        } else if (*str == '+') {
            ++str;
        }
        
        // Get digits of exponent, if any.
        
        for (expon = 0; valid_digit(*str); ++str) {
            expon = expon * 10 + (*str - '0');
        }
        if (expon > 308) expon = 308;
        
        // Calculate scaling factor.
        
        while (expon >= 50) { scale *= 1E50; expon -= 50; }
        while (expon >=  8) { scale *= 1E8;  expon -=  8; }
        while (expon >   0) { scale *= 10.0; expon -=  1; }
    }
    
    // Return signed and scaled floating point result.
    
    return sign * (frac ? (value / scale) : (value * scale));
}


//-----------------------------------------------------------------------------
template<typename ... ARGS>
String String::fromFormat(const char* format, ARGS ... args)
{
    String ret;
    ret.format(format, args ...);
    return ret;
}
    

//-----------------------------------------------------------------------------
int64 String::atoi64(const char* str)
{
    #ifdef WIN32
    return _atoi64(str);
    #else
    return atoll(str);
    #endif
}


//-----------------------------------------------------------------------------
uint64 String::atoui64(const char* str)
{
    #ifdef WIN32
    return _strtoui64(str, NULL, 10);
    #else
    return strtoull(str, NULL, 10);
    #endif
}
    
}
