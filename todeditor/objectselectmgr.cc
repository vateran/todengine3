#include "todeditor/objectselectmgr.h"
#include "tod/serializer.h"
#include "tod/kernel.h"
namespace tod::editor
{


//-----------------------------------------------------------------------------
const char* ObjectSelectMgr::EVENT_SELECTION_CHANGED = "EVENT_SELECTION_CHANGED";


//-----------------------------------------------------------------------------
void ObjectSelectMgr::setSelections(const NodeSelections& selections)
{
    this->selections = selections;
    
    this->dispatchEvent(EVENT_SELECTION_CHANGED);
    
    Serializer s;
    s.serializeToJsonFile(Kernel::instance()->lookup("/scene"), "autosave.json");
}


//-----------------------------------------------------------------------------
const NodeSelections& ObjectSelectMgr::getSelections() const
{
    return this->selections;
}

}
