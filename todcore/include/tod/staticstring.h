#pragma once
#include "tod/string.h"
namespace tod
{

class StaticString
{
public:
    StaticString();
    StaticString(const char* str);
    StaticString(const String& str);

    inline size_t size() const;
    inline int32 hash() const;

    bool empty() const;
    size_t length() const;
    const char* c_str() const;

    inline bool operator < (const StaticString& rhs) const;
    inline bool operator == (const char* str) const;
    inline bool operator == (const String& str) const;

private:
    int32 hashValue;
};


//-----------------------------------------------------------------------------
size_t StaticString::size() const
{
    return this->length();
}


//-----------------------------------------------------------------------------
int32 StaticString::hash() const
{
    return this->hashValue;
}


//-----------------------------------------------------------------------------
bool StaticString::operator < (const StaticString& rhs) const
{
    return this->hashValue < rhs.hashValue;
}


//-----------------------------------------------------------------------------
bool StaticString::operator == (const char* str) const
{
    return this->hashValue == String::hash(str);
}


//-----------------------------------------------------------------------------
bool StaticString::operator == (const String& str) const
{
    return this->hashValue == str.hash();
}


//-----------------------------------------------------------------------------
inline bool operator == (const StaticString& l, const StaticString& r)
{
    return l.hash() == r.hash();
}

}


//-----------------------------------------------------------------------------
namespace std
{
    template <>
    struct hash<tod::StaticString>
    {
        inline std::size_t operator() (const tod::StaticString& str) const
        {
            return str.hash();
        }
    };
}