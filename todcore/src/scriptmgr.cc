#include "tod/scriptmgr.h"
namespace tod
{

//-----------------------------------------------------------------------------
bool ScriptMgr::run
(const String& processor_name,
 const String& script, String* result)
{
    auto i = this->processors.find(processor_name.hash());
    if (this->processors.end() == i) TOD_RETURN_TRACE(false);
    
    return i->second->run(script, result);
}


//-----------------------------------------------------------------------------
bool ScriptMgr::runFile(const String& file_path, String* result)
{
    auto p = file_path.rfind(".");
    if (p == String::npos) TOD_RETURN_TRACE(false);
    String ext { file_path.substr(p + 1, -1) };
    
    auto i = this->processors.find(ext.hash());
    if (this->processors.end() == i) TOD_RETURN_TRACE(false);
    
    return i->second->runFile(file_path, result);
}
    

//-----------------------------------------------------------------------------
void ScriptMgr::registerProcessor(IScriptProcessor* processor)
{
    this->processors.insert(
        std::make_pair(processor->getName().hash(), processor));
}
    
}
