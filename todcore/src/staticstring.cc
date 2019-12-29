#include "tod/precompiled.h"
#include "tod/singleton.h"
#include "tod/staticstringstorage.h"
#include "tod/staticstring.h"
namespace tod
{

//-----------------------------------------------------------------------------
StaticString::StaticString()
: hashValue(StaticStringStorage::instance()->addString(""))
{
}

//-----------------------------------------------------------------------------
StaticString::StaticString(const char* str)
: hashValue(StaticStringStorage::instance()->addString(str))
{
}


//-----------------------------------------------------------------------------
StaticString::StaticString(const String& str)
: hashValue(StaticStringStorage::instance()->addString(str))
{
}


//-----------------------------------------------------------------------------
bool StaticString::empty() const
{
    if (0 == this->hashValue)
    {
        return true;
    }

    return StaticStringStorage::instance()->getString(this->hashValue).empty();
}


//-----------------------------------------------------------------------------
size_t StaticString::length() const
{
    return StaticStringStorage::instance()->getString(this->hashValue).length();
}


//-----------------------------------------------------------------------------
const char* StaticString::c_str() const
{
    return StaticStringStorage::instance()->getString(this->hashValue).c_str();
}

}