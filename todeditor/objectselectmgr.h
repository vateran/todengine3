#pragma once
#include "tod/object.h"
#include "tod/eventdispatcher.h"
#include "todeditor/common.h"
namespace tod::editor
{

class ObjectSelectMgr : public EventDispatcher<SingletonDerive<ObjectSelectMgr, Object>>
{
public:
    static const char* EVENT_SELECTION_CHANGED;
    
public:
    void setSelections(const NodeSelections& selections);
    
    const NodeSelections& getSelections() const;
    
private:
    NodeSelections selections;
};

}
