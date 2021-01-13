#include "tod/precompiled.h"
#include "tod/string.h"
#include "tod/staticstring.h"
#if defined(TOD_PLATFORM_WINDOWS)
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#endif
namespace tod
{

//-----------------------------------------------------------------------------
String::String(const StaticString& str)
:BaseString(str.c_str())
{
}

//-----------------------------------------------------------------------------
void String::replace(const String& from, const String& to)
{
    size_t index = 0;
    while ((index = this->find(from, index)) != String::npos)
    {
        this->BaseString::replace(index, from.size(), to);
        index += from.size();
    }
}


//-----------------------------------------------------------------------------
int32 String::hash() const
{
    return String::hash(this->c_str(), this->size());
}
    

//-----------------------------------------------------------------------------
String String::extractFileExtension() const
{
    auto p = this->rfind(S("."));
    if (String::npos == p) return String();
    return String(*this, p + 1, -1);
}


//-----------------------------------------------------------------------------
String String::extractPath() const
{
    auto p = this->rfind(S("/"));
    if (String::npos == p) return S("");
    return String(*this, 0, p);
}


//-----------------------------------------------------------------------------
String String::extractFileName() const
{
    auto p = this->rfind(S("/"));
    return String(*this, p + 1, String::npos);
}


//-----------------------------------------------------------------------------
String String::normalizePath() const
{
    String ret;
    ret.resize(this->size());
    for (size_t i = 0; i < this->size(); ++i)
    {
        if (this->at(i) == S('\\'))
        {
            ret.at(i) = S('/');
        }
        else
        {
            ret.at(i) = this->at(i);
        }
    }
    return ret;
}
    

//-----------------------------------------------------------------------------
String& String::lower()
{
    std::transform(this->begin(), this->end(), this->begin(), ::tolower);
    return *this;
}


//-----------------------------------------------------------------------------
String& String::upper()
{
    std::transform(this->begin(), this->end(), this->begin(), ::toupper);
    return *this;
}
    
    
//-----------------------------------------------------------------------------
int32 String::atoi(const char* str)
{
    int32 val = 0;
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


//-----------------------------------------------------------------------------
template <>
double String::atof(const char* str)
{
    int32 frac;
    double sign, value, scale;

    // Skip leading white space, if any.
    while (std::isspace(*str)) ++str;

    // Get sign, if any.
    sign = 1.0;
    if (*str == S('-'))
    {
        sign = -1.0;
        ++str;
    }
    else if (*str == S('+'))
    {
        ++str;
    }

    // Get digits before decimal point32 or exponent, if any.
    for (value = 0.0; std::isdigit(*str); ++str)
    {
        value = value * 10.0 + (*str - S('0'));
    }

    // Get digits after decimal point, if any.    
    if (*str == '.')
    {
        double pow10 = 10.0;
        ++str;
        while (std::isdigit(*str))
        {
            value += (*str - S('0')) / pow10;
            pow10 *= 10.0;
            ++str;
        }
    }

    // Handle exponent, if any.    
    frac = 0;
    scale = 1.0;
    if ((*str == S('e')) || (*str == S('E')))
    {
        uint32 expon;

        // Get sign of exponent, if any.
        ++str;
        if (*str == S('-'))
        {
            frac = 1;
            ++str;
        }
        else if (*str == S('+'))
        {
            ++str;
        }

        // Get digits of exponent, if any.
        for (expon = 0; std::isdigit(*str); ++str)
        {
            expon = expon * 10 + (*str - S('0'));
        }
        if (expon > 308) expon = 308;

        // Calculate scaling factor.        
        while (expon >= 50) { scale *= 1E50; expon -= 50; }
        while (expon >= 8) { scale *= 1E8;  expon -= 8; }
        while (expon >   0) { scale *= 10.0; expon -= 1; }
    }

    // Return signed and scaled floating point32 result.    
    return sign * (frac ? (value / scale) : (value * scale));
}


template <>
float String::atof<float>(const char* str)
{
    int32 frac;
    float sign, value, scale;

    // Skip leading white space, if any.
    while (std::isspace(*str)) ++str;

    // Get sign, if any.
    sign = 1.0;
    if (*str == S('-'))
    {
        sign = -1.0f;
        ++str;
    }
    else if (*str == S('+'))
    {
        ++str;
    }

    // Get digits before decimal point32 or exponent, if any.
    for (value = 0.0; std::isdigit(*str); ++str)
    {
        value = value * 10.0f + (*str - S('0'));
    }

    // Get digits after decimal point, if any.    
    if (*str == S('.'))
    {
        float pow10 = 10.0f;
        ++str;
        while (std::isdigit(*str))
        {
            value += (*str - S('0')) / pow10;
            pow10 *= 10.0f;
            ++str;
        }
    }

    // Handle exponent, if any.    
    frac = 0;
    scale = 1.0;
    if ((*str == S('e')) || (*str == S('E')))
    {
        uint32 expon;

        // Get sign of exponent, if any.
        ++str;
        if (*str == S('-'))
        {
            frac = 1;
            ++str;
        }
        else if (*str == S('+'))
        {
            ++str;
        }

        // Get digits of exponent, if any.
        for (expon = 0; std::isdigit(*str); ++str)
        {
            expon = expon * 10 + (*str - S('0'));
        }
        if (expon > 308) expon = 308;

        // Calculate scaling factor.      
        while (expon >= 8) { scale *= 1E8;  expon -= 8; }
        while (expon >  0) { scale *= 10.0; expon -= 1; }
    }

    // Return signed and scaled floating point32 result.    
    return sign * (frac ? (value / scale) : (value * scale));
}


//-----------------------------------------------------------------------------
int32 String::hash(const char* str, size_t len)
{
    uint32 hash = 5381;

    if (len > 0)
    {   
        for (auto i = 0u; i < len; ++str, ++i)
        {
            hash = ((hash << 5) + hash) + (*str);
        }
    }
    else
    {
        while (*str != 0)
        {
            hash = ((hash << 5) + hash) + (*str);
            ++str;
        }
    }

    return hash;
}


//-----------------------------------------------------------------------------
size_t String::findString(const char* str, const char* find_str)
{
    const char* p = strstr(str, find_str);
    if (nullptr == p)
    {
        return String::npos;
    }

    return static_cast<size_t>(p - str);
}


//-----------------------------------------------------------------------------
WString String::toUTF8() const
{
#if defined (TOD_PLATFORM_WINDOWS)
    WString ret;
    ret.resize(this->size());
    MultiByteToWideChar(
          CP_ACP, 0
        , this->data(), static_cast<int32>(this->size())
        , &ret[0], static_cast<int32>(ret.size()));
    return ret;
#else
#endif
}


}
