#pragma once
#include <bitset>
#include <functional>
#include "tod/buffer.h"
#include "tod/object.h"
#include "tod/promise.h"
namespace tod
{

//!@ingroup FileSystem
//!@brief TodEngine 에서 파일을 핸들링
class FileSystem : public SingletonDerive<FileSystem, Object>
{
public:
    typedef std::function<bool(Buffer*)> LoadComplete;
    typedef String DirName;
    typedef String FileName;
    typedef std::function<bool(const DirName&, const FileName)> IterateFilesCB;
    
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
    /*class Result
    {
    public:
    };

    typedef Promise<Result, FileSystem> ResultPromise;*/
    
public:
    FileSystem();
    virtual~FileSystem();
    
    //!@brief 파일을 찾을 search path 를 지정
    void setSearchPaths(const std::list<String>& paths);
    const std::list<String>& getSearchPaths() { return this->searchPaths; }
    
    String getRelativePath(const String& path);

    /*const ResultPromise& load(
          const String& uri
        , const LoadOption& option = LoadOption()) const;*/

    //!@brief URI로 지정된 파일, 아카이브, 원격지 데이터등을 읽고,
    //!완료되면 callback으로 데이터를 반환
    //!@param uri protocol://path 로 지정된 URI
    //!       file://, zip://, http:// 이 될 수 있음
    //!       프로토콜이 지정되지 않으면 file:// 이 default
    //!@param callback 로딩이 완료되면 호출되는 callback 함수
    //!@param option LoadOption 을 조합
    //!@see setSearchPaths
    bool load(
          const String& uri
        , const LoadComplete& callback
        , const LoadOption& option=LoadOption()) const;
    //!@breif URI로 지정된 리소스를 로딩한다
    //!@param option 이 메서드에서 async 는 무시됨
    bool load(
          const String& uri
        , Buffer* output
        , const LoadOption& option=LoadOption()) const;
    bool save(const String& uri, const void* data, uint64 size);

    //!@brief URI로 지정된 파일이 존재하는지 알아낸다
    bool isExists(const String& uri) const;
    //!@brief URI로 지정된 파일의 크기를 알아낸다
    int64 getFileSize(const String& uri) const;
    //!@brief current working directory 를 알아낸다
    String getCurrentWorkingDirectory() const;
    //@breif 지정된 directory 를 돌면서 callback 을 호출해준다. file:// 프로토콜만됨
    bool iterateFiles(
          const String& uri
        , const IterateFilesCB& callback
        , bool recursive);
    
public:
    class IProtocol
    {
    public:
        typedef std::function<void(Buffer&)> LoadComplete;
        
    public:
        IProtocol(const String& protocol):protocol(protocol) {}
        virtual~IProtocol() {}
        
        //!@brief 리소스를 로딩한다.
        //!실제 구현에서는 async loading 을 위해 re-enterance func 으로 제작되어야 한다
        virtual bool load(
              const String& path
            , Buffer* output
            , const LoadOption& option)=0;
        virtual bool save(
              const String& path
            , const void* data
            , uint64 size)=0;
        virtual bool isExists(const String& path) = 0;
        virtual bool iterateFiles(
              const String& path
            , const IterateFilesCB& callback
            , bool recursive) = 0;
        const String& getProtocol() const { return this->protocol; }
        
    private:
        String protocol;
    };
    
private:
    void reigster_protocol(IProtocol* protocol);
    void split_protocol_and_path(
          const String& uri
        , String& protocol
        , String& path) const;
    bool load(
          const String& protocol
        , const String& path
        , Buffer* output
        , const LoadOption& option) const;
    
private:
    typedef std::vector<IProtocol*> Protocols;
    Protocols protocols;
    std::list<String> searchPaths;
};

}
