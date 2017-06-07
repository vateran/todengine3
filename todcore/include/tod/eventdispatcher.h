#pragma once
#include <algorithm>
#include <list>
#include "tod/params.h"
namespace tod
{


//-----------------------------------------------------------------------------
typedef std::function<void(Params*)> EventCallback;
class NullClass {};
    
    
//-----------------------------------------------------------------------------
//!@ingroup Core
//!@brief Object에서 발생하는 Event 를 핸들링하기 위한 Handler(C++11 labma)를 등록.
//!Event를 발생시킨다
//!@code
//!auto node = Kernel::instance()->create("Node", "/node1");
//!node->addEventHandler(Node::EVENT_ADDCHILD, [](Params*)
//!{
//!     //Node에서 EVENT_ADDCHILD 이벤트가 발생되면 Callback 호출됨 
//!});
//!@endcode
template <typename BASE=NullClass>
class EventDispatcher : public BASE
{
public:
    template <typename ... ARGS>
    EventDispatcher(const ARGS ... args):BASE(args ...) {}
    void addEventHandler(const char* event_type,
                         void* target,
                         const EventCallback& callback);
    void removeEventHandler(const char* event_type, void* target);
    void removeAllEventHandler(void* target);
    void clearEventHandler();
    void dispatchEvent(const char* event_type, Params* params=nullptr);
    template <typename ... ARGS>
    void dispatchEvent(const char* event_type, const ARGS& ... args);
    bool isEmptyEventHandler() { return this->handlers.empty(); }
    
private:
    struct HandlerInfo
    {
        HandlerInfo(const char* event_name,
                    void* target,
                    const EventCallback& callback):
        eventName(event_name)
        , target(target)
        , callback(callback) {}
        
        const char* eventName;
        void* target;
        EventCallback callback;
    };
    typedef std::shared_ptr<HandlerInfo> HandlerInfoPtr;
    typedef std::list<HandlerInfoPtr> Handlers;
    Handlers handlers;
};


//-----------------------------------------------------------------------------
template <typename T>
void EventDispatcher<T>::addEventHandler
(const char* event_name, void* target, const EventCallback& callback)
{
    auto iter = std::find_if(this->handlers.begin(), this->handlers.end(),
        [event_name, target](const HandlerInfoPtr& info)
        {
            return (info->eventName == event_name
                    && info->target == target);
        });
    
    //원래 있다면 callback 함수만 덮어쓰기
    if (this->handlers.end() != iter)
    {
        (*iter)->callback = callback;
    }
    else
    {
        //없으면 새로 생성
        this->handlers.push_back(HandlerInfoPtr(
            new HandlerInfo(event_name, target, callback)));
    }
}


//-----------------------------------------------------------------------------
template <typename T>
void EventDispatcher<T>::removeEventHandler
(const char* event_name, void* target)
{
    for (auto i=this->handlers.begin();i!=this->handlers.end();++i)
    {
        if ((*i)->eventName == event_name
            && (*i)->target == target)
        {
            this->handlers.erase(i);
            break;
        }
    }
}


//-----------------------------------------------------------------------------
template <typename T>
void EventDispatcher<T>::removeAllEventHandler(void* target)
{
    this->handlers.remove_if([target](const HandlerInfoPtr& info)
    { return info->target == target; });
}


//-----------------------------------------------------------------------------
template <typename T>
void EventDispatcher<T>::clearEventHandler()
{
    this->handlers.clear();
}


//-----------------------------------------------------------------------------
template <typename T>
void EventDispatcher<T>::dispatchEvent
(const char* event_name, Params* params)
{
    if (this->handlers.empty())
    {
        if (params) delete params;
        return;
    }
    
    for (auto& i : this->handlers)
    {
        if (i->eventName != event_name) continue;
        i->callback(params);
    }
    
    if (params) delete params;
}
    

//-----------------------------------------------------------------------------
template <typename T>
template <typename ... ARGS>
void EventDispatcher<T>::dispatchEvent
(const char* event_type, const ARGS& ... args)
{
    this->dispatchEvent(event_type, Params::newParam(args ...));
}



}
