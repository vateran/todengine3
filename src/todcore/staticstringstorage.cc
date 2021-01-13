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
        //hash �浹!
        TOD_ASSERT(
              i->second == str
            , "StaticString �߰��Ҷ� hash �浹��\n����[%s]�ű�[%s]"
            , i->second.c_str(), str.c_str());
    }

    return hash;
}


//-----------------------------------------------------------------------------
const String& StaticStringStorage::getString(int32 hash) const
{
    std::lock_guard<std::mutex> guard(this->lock);
    auto i = this->strings.find(hash);
    TOD_ASSERT(this->strings.end() != i, "��ϵ� StaticString�� ����[%d]", hash);
    return i->second;
}

}