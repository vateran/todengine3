﻿#pragma once
#include "tod/string.h"
#include "tod/staticstring.h"
namespace tod
{

//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief C/C++ native 타입에 따른 String 변환 처리
template <typename TYPE>
class StringConv
{
};


//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief bool 타입에 대한 String 변환
template <>
class StringConv<bool>
{
public:
    static bool fromString(const String& value)
    {
        return (value[0] == 't' || value[0] == 'T');
    }
    static void toString(bool value, String& s)
    {
        static String s_true("true"), s_false("false");
        if(value) s = s_true;
        else s = s_false;
    }
};


//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief int32 타입에 대한 String 변환
template <>
class StringConv<int>
{
public:
    static int32 fromString(const String& value)
    {
        return String::atoi(value.c_str());
    }
    static void toString(int32 value, String& s)
    {
        s.format("%d", value);
    }
};


//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief int64 타입에 대한 String 변환
template <>
class StringConv<int64>
{
public:
    static int64 fromString(const String& value)
    {
        return String::atoi64(value.c_str());
    }
    static void toString(int64 value, String& s)
    {
        s.format("%ll", value);
    }
};


//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief float 타입에 대한 String 변환
template <>
class StringConv<float>
{
public:
    static float fromString(const String& value)
    {
        return String::atof<float>(value.c_str());
    }
    static void toString(float value, String& s)
    {
        s.format("%.3f", value);
    }
};


//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief String 타입에 대한 String 변환
template <>
class StringConv<String>
{
public:
    static const String& fromString(const String& value)
    {
        return value;
    }
    static void toString(const String& value, String& s)
    {
        s = value;
    }
};


//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief const String& 타입에 대한 String 변환
template <>
class StringConv<const String&>
{
public:
    static const String& fromString(const String& value)
    {
        return value;
    }
    static void toString(const String& value, String& s)
    {
        s = value;
    }
};


//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief const String& 타입에 대한 String 변환
template <>
class StringConv<StaticString>
{
public:
    static StaticString fromString(const String& value)
    {
        return value;
    }
    static void toString(StaticString value, String& s)
    {
        s = value;
    }
};


//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief const String& 타입에 대한 String 변환
template <>
class StringConv<const StaticString&>
{
public:
    static StaticString fromString(const String& value)
    {
        return value;
    }
    static void toString(const StaticString& value, String& s)
    {
        s = value;
    }
};


//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief char* 타입에 대한 String 변환
template <>
class StringConv<char*>
{
public:
    static const char* fromString(const String& value)
    {
        return value.c_str();
    }
    static void toString(const char* value, String& s)
    {
        s = value;
    }
};


//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief const char* 타입에 대한 String 변환
template <>
class StringConv<const char*>
{
public:
    static const char* fromString(const String& value)
    {
        return value.c_str();
    }
    static void toString(const char* value, String& s)
    {
        s = value;
    }
};


//-----------------------------------------------------------------------------
/*template <>
class StringConv<Color>
{
public:
    static Color fromString(const char* value)
    {
        Color c;
        Tokenizer tok;
        int32 n=tok.Extract(value, "", ",[]");
        if(n!=4) return c;
        for(int32 i=0;i<n;++i)
        {
            c.c[i] = Util::fast_atoi(tok.GetToken(i));
        }
        return c;
    }
    static void toString(const Color& c, std::string& s)
    {
        char buf[64];
        snprintf(buf, sizeof(buf), "[%d,%d,%d,%d]", c.r, c.g, c.b, c.a);
        s = buf;
    }
};


//-----------------------------------------------------------------------------
template <>
class StringConv<const Color&>
{
public:
    static Color fromString(const char* value)
    {
        Color c;
        Tokenizer tok;
        int32 n=tok.Extract(value, "", ",[]");
        if(n!=4) return c;
        for(int32 i=0;i<n;++i)
        {
            c.c[i] = Util::fast_atoi(tok.GetToken(i));
        }
        return c;
    }
    static void toString(const Color& c, std::string& s)
    {
        char buf[64];
        snprintf(buf, sizeof(buf), "[%d,%d,%d,%d]", c.r, c.g, c.b, c.a);
        s = buf;
    }
};


//-----------------------------------------------------------------------------
template <>
class StringConv<Rect>
{
public:
    static Rect fromString(const char* value)
    {
        Rect r;
        Tokenizer tok;
        int32 n=tok.Extract(value, "", ",[]");
        if(n!=4) return r;
        for(int32 i=0;i<n;++i)
        {
            r.r[i] = static_cast<float>(Util::fast_atof<double>(tok.GetToken(i)));
        }
        return r;
    }
    static void toString(const Rect& r, std::string& s)
    {
        char buf[64];
        snprintf(buf, sizeof(buf), "[%d,%d,%d,%d]", (int)r.x, (int)r.y, (int)r.w, (int)r.h);
        s = buf;
    }
};


//-----------------------------------------------------------------------------
template <>
class StringConv<const Rect&>
{
public:
    static Rect fromString(const char* value)
    {
        Rect r;
        Tokenizer tok;
        int32 n=tok.Extract(value, "", ",[]");
        if(n!=4) return r;
        for(int32 i=0;i<n;++i)
        {
            r.r[i] = static_cast<float>(Util::fast_atof<double>(tok.GetToken(i)));
        }
        return r;
    }
    static void toString(const Rect& r, std::string& s)
    {
        char buf[64];
        snprintf(buf, sizeof(buf), "[%d,%d,%d,%d]", (int)r.x, (int)r.y, (int)r.w, (int)r.h);
        s = buf;
    }
};*/

}
