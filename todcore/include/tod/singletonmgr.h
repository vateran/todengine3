#pragma once
#include <list>
namespace tod
{

class ISingleton;
    
//!@ingroup Core
//!@brief TodEngine에서 사용하는 Singleton들을 관리
class SingletonMgr
{
public:
    ~SingletonMgr();
    
    //!@brief 등록된 Singleton객체를 모두 파괴
    void clear();
    //!@brief Singleton객체를 등록
    void push_back(ISingleton* s);
    
public:
    static SingletonMgr* instance();
    
private:
    typedef std::list<ISingleton*> Singletons;
    Singletons singletons;
};

}
