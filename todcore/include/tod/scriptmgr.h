#pragma once
#include "tod/object.h"
#include "tod/singleton.h"
namespace tod
{
    
//!@ingroup Scripting
//!@brief 스크립트 처리기 인터페이스
class IScriptProcessor
{
public:
    virtual~IScriptProcessor() {}
    
    virtual bool run(const String& script, String* result=nullptr)=0;
    virtual bool runFile(const String& file_path, String* result=nullptr)=0;
    
    //!@brief Sciprt 처리기 이름
    const String& getName() const { return this->name; }
    //!@brief Sciprt 가 처리할 확장자를 반환
    const String& getFileExt() const { return this->ext; }
    
private:
    String name;
    String ext;
        
};
    

//!@ingroup Scripting
//!@brief Python을 사용하여 스크립트를 처리
class PythonScriptProcessor : public IScriptProcessor
{
public:
    bool run(const String& script, String* result=nullptr) override
    {
        return true;
    }
    bool runFile(const String& file_path, String* result=nullptr) override
    {
        return true;
    }
};
    

//!@ingroup Scripting
//!@brief IScriptProcessor 를 관리
class ScriptMgr : public Singleton<ScriptMgr>
{
public:
    typedef std::unordered_map<int, IScriptProcessor*> Processors;
    
public:
    bool run(const String& processor_name,
             const String& script,
             String* result=nullptr);
    bool runFile(const String& file_path, String* result=nullptr);
    
    void registerProcessor(IScriptProcessor* processor);
    
private:
    Processors processors;
    
    
};
    
}
