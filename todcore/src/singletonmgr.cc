#include "tod/singletonmgr.h"
#include "tod/singleton.h"
namespace tod
{
    
//-----------------------------------------------------------------------------
SingletonMgr* SingletonMgr::instance()
{
    static SingletonMgr s_instance;
    return &s_instance;
}
    

//-----------------------------------------------------------------------------
SingletonMgr::~SingletonMgr()
{
    this->clear();
}
    

//-----------------------------------------------------------------------------
void SingletonMgr::clear()
{
    this->singletons.reverse();
    for (auto& i : this->singletons) delete i;
    this->singletons.clear();
}

//-----------------------------------------------------------------------------
void SingletonMgr::push_back(ISingleton* s)
{
    this->singletons.push_back(s);
}
    
}
