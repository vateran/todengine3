#pragma once
#include <functional>
#include <unordered_map>
#include "tod/singleton.h"
#include "tod/string.h"
namespace tod
{


//!@ingroup FileSystem
//!@brief TodEngine 에서 파일을 핸들링
class FileSystem : public Singleton<FileSystem>
{
public:
    typedef std::vector<char> Data;
    typedef std::function<bool(Data&)> LoadComplete;
    
    //!@brief FileSystem::load 에서 파일 로딩시 옵션을 지정
    //!@code
    //!FileSystem::instance()->load("uri", FileSystem::LoadOption().async());
    //!return true;
    //!@endcode
    struct LoadOption
    {
        //!@brief 비동기로 로딩
        LoadOption& aync() { this->flags[ASYNC] = true; return *this; }
        //!@brief 반환값은 데이터끝에 null이 포함
        LoadOption& string() { this->flags[STRING] = true; return *this; }
        
        bool isAsync() const { return this->flags[ASYNC]; }
        bool isString() const { return this->flags[STRING]; }
        
    private:
        enum
        {
            ASYNC,
            STRING,
            
            MAX
        };
        std::bitset<MAX> flags;
    };
    
public:
    FileSystem();
    virtual~FileSystem();
    
    //!@brief 파일을 찾을 search path 를 지정
    void setSearchPaths(const std::list<String>& paths);
    const std::list<String>& getSearchPaths() { return this->searchPaths; }
    
    //!@brief URI로 지정된 파일, 아카이브, 원격지 데이터등을 읽고,
    //!완료되면 callback으로 데이터를 반환
    //!@param uri protocol://path 로 지정된 URI
    
    //!       file://, zip://, http:// 이 될 수 있음
    //!       프로토콜이 지정되지 않으면 file:// 이 default
    //!@param callback 로딩이 완료되면 호출되는 callback 함수
    //!@param option LoadOption 을 조합
    //!@see setSearchPaths
    bool load(const String& uri,
              const LoadComplete& callback,
              const LoadOption& option=LoadOption()) const;
    
    //!@brief URI로 지정된 파일
    int64 getFileSize(const String& uri) const;
    //!@brief current working directory 를 알아낸다
    String getCurrentWorkingDirectory() const;
    
public:
    class ILoader
    {
    public:
        typedef std::vector<char> Data;
        typedef std::function<void(Data&)> LoadComplete;
        
    public:
        ILoader(const String& protocol):protocol(protocol) {}
        virtual~ILoader() {}
        
        //!@brief 리소스를 로딩한다.
        //!실제 구현에서는 async loading 을 위해 re-enterance func 으로 제작되어야 한다
        virtual bool load(const String& path,
                          const LoadComplete& callback,
                          const LoadOption& option) =0;
        const String& getProtocol() const { return this->protocol; }
        
    private:
        String protocol;
    };
    
private:
    void reigster_loader(ILoader* file_loader);
    
private:
    typedef std::unordered_map<int, ILoader*> Loaders;
    Loaders loaders;
    std::list<String> searchPaths;
};

}
