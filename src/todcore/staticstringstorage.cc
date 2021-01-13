#include "tod/precompiled.h"
#include "tod/singleton.h"
#include "tod/staticstringstorage.h"
namespace tod
{

//-----------------------------------------------------------------------------
StaticStringStorage::StaticStringStorage()
{
}


//-----------------------------------------------------------------------------
int32 StaticStringStorage::addString(const String& str)
{
    std::lock_guard<std::mutex> guard(this->lock);
    int32 hash = str.hash();
    auto i = this->strings.find(hash);
    if (this->strings.end() == i)
    {
        this->strings.emplace(hash, str);
    }
    else
    {
        //hash 충돌!
        TOD_ASSERT(
              i->second == str
            , "StaticString 추가할때 hash 충돌남\n이전[%s]신규[%s]"
            , i->second.c_str(), str.c_str());
    }

    return hash;
}


//-----------------------------------------------------------------------------
const String& StaticStringStorage::getString(int32 hash) const
{
    std::lock_guard<std::mutex> guard(this->lock);
    auto i = this->strings.find(hash);
    TOD_ASSERT(this->strings.end() != i, "등록된 StaticString이 없음[%d]", hash);
    return i->second;
}

}