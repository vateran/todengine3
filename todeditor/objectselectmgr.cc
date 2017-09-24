class ObjectSelectMgr : public EventDispatcher<SingletonDerive<ObjectSelectMgr, Object>>
{
public:
    static const char* EVENT_SELECTION_CHANGED;
    
public:
    void setSelections(const NodeSelections& selections)
    {
        this->selections = selections;
        
        this->dispatchEvent(EVENT_SELECTION_CHANGED);
        
        Serializer s;
        s.serializeToJsonFile(Kernel::instance()->lookup("/scene"), "autosave.json");
    }
    
    const NodeSelections& getSelections() const
    {
        return this->selections;
    }
    
private:
    NodeSelections selections;
};
const char* ObjectSelectMgr::EVENT_SELECTION_CHANGED = "EVENT_SELECTION_CHANGED";
