#pragma once
#include "tod/singleton.h"
#include "tod/objref.h"
namespace tod
{

class AutoReleasePool : public Singleton<AutoReleasePool>
{
public:
    void add(Object* object)
    {
        this->objects.push_back(object);
    }
    void release()
    {
    }
    
private:
    typedef std::list<ObjRef<Object>> Objects;
    Objects objects;
};

}
