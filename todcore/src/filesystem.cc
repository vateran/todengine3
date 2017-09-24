#include "tod/platformdef.h"
#include <vector>
#include <thread>
#include <fstream>
#ifdef PLATFORM_WINDOWS
#include <WinSock2.h>
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif
#include <curl/curl.h>
#include "tod/filesystem.h"
namespace tod
{

//-----------------------------------------------------------------------------
class PosixProtocol : public FileSystem::IProtocol
{
public:
    PosixProtocol():IProtocol(S("file")) {}
    
    bool load(const String& path,
              Data& output,
              const FileSystem::LoadOption& option)  override;
    bool save(const String& path, const char* data, uint64 size) override;
};
    
    
//-----------------------------------------------------------------------------
class ZipProtocol : public FileSystem::IProtocol
{
public:
    ZipProtocol():IProtocol(S("zip")) {}
    
    bool load(const String& path,
              Data& output,
              const FileSystem::LoadOption& option)  override
    {
        TOD_RETURN_TRACE(false);
    }
    bool save(const String& path, const char* data, uint64 size) override
    {
        TOD_RETURN_TRACE(false);
    }
};


//-----------------------------------------------------------------------------
class HttpProtocol : public FileSystem::IProtocol
{
public:
    enum
    {
        LOW_SPEED_LIMIT = 1L,
        LOW_SPEED_TIME = 5L,
    };
    
public:
    HttpProtocol():IProtocol(S("http")) {}
    
    bool load(const String& path,
              Data& output,
              const FileSystem::LoadOption& option) override
    {
        RequestData request_data;
        request_data.data = &output;
        
        auto curl = curl_easy_init();
        if (!curl) TOD_RETURN_TRACE(false);
        
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                         HttpProtocol::write_data_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &request_data);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, LOW_SPEED_LIMIT);
        curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, LOW_SPEED_TIME);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION,
                         HttpProtocol::progress_callback);
        curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &request_data);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, nullptr);
        
        CURLcode code = CURLE_OK;
        String url { S("http://") };
        url += path;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        code = curl_easy_perform(curl);
        if(CURLE_OK != code)
        {
            TOD_THROW_EXCEPTION("@f(@l):curl_easy_perform: %s",
                                curl_easy_strerror(code));
            curl_easy_cleanup(curl);
            TOD_RETURN_TRACE(false);
        }
        
        request_data.responseCode = 0;
        code = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE,
                                 &request_data.responseCode);
        if(code != CURLE_OK || !(request_data.responseCode >= 200 &&
                                 request_data.responseCode < 300))
        {
            TOD_THROW_EXCEPTION("@f(@l):Curl curl_easy_getinfo failed: %s",
                                curl_easy_strerror(code));
            curl_easy_cleanup(curl);
            TOD_RETURN_TRACE(false);
        }
        curl_easy_cleanup(curl);
        
        if (option.isString())
            request_data.data->insert(request_data.data->end(), '0');
        
        return true;
    }
    bool save(const String& path, const char* data, uint64 size) override
    {
        TOD_RETURN_TRACE(false);
    }
    
private:
    struct RequestData
    {
        RequestData():
          responseCode(0)
        , recvBlockSize(0)
        , recvSize(0)
        , totalSize(0) {}
        
        Data* data;
        int responseCode;
        size_t recvBlockSize;
        size_t recvSize;
        size_t totalSize;
    };
    static size_t write_data_callback
    (void *ptr, size_t size, size_t nmemb, void *userdata)
    {
        auto self = reinterpret_cast<RequestData*>(userdata);
        size_t sizes = size * nmemb;
        self->data->insert(self->data->end(), (char*)ptr, (char*)ptr+sizes);
        self->recvBlockSize += sizes;
        self->recvSize += sizes;
        return sizes;
    }
    static int progress_callback
    (void *userdata, double dltotal, double dlnow, double ultotal, double ulnow)
    {
        auto self = reinterpret_cast<RequestData*>(userdata);
        self->totalSize = static_cast<int>(dltotal);
        return 0;
    }
};
    

//-----------------------------------------------------------------------------
FileSystem::FileSystem()
{
    //Protocol 들을 등록해준다
    this->reigster_protocol(new PosixProtocol);
    this->reigster_protocol(new ZipProtocol);
    //this->reigster_protocol(new HttpProtocol);
    this->searchPaths.push_back(S("."));
}
    

//-----------------------------------------------------------------------------
FileSystem::~FileSystem()
{
    for (auto& fl : this->protocols) delete fl;
}


//-----------------------------------------------------------------------------
void FileSystem::setSearchPaths(const std::list<String>& paths)
{
    this->searchPaths = paths;
    this->searchPaths.push_front(this->getCurrentWorkingDirectory());
}


//-----------------------------------------------------------------------------
String FileSystem::getRelativePath(const String& path)
{
    if (path.empty()) return String();
    
    
    //search path 를 가장 길이가 긴 순으로 정렬
    std::vector<String> sorted;
    sorted.reserve(this->searchPaths.size());
    for (auto& i : this->searchPaths)
        sorted.push_back(i);
    std::sort(sorted.begin(), sorted.end(),
        [](const String& l, const String& r)
        { return l.length() > r.length(); });
    
    for (auto& spath : sorted)
    {
        auto p = path.find(spath);
        if (String::npos == p) continue;
        return path.substr(spath.size() + 1, -1);
    }
    
    return path;
}

    
//-----------------------------------------------------------------------------
bool FileSystem::load
(const String& uri, const LoadComplete& callback,
 const LoadOption& option) const
{
    String protocol, path;
    this->split_protocol_and_path(uri, protocol, path);
    
    if (option.isAsync())
    {
        //TODO: 이거 main thread 에서 queue 동기화 해서 처리해야한다
        std::thread([this, protocol, path, callback, option]()
        {
            Data output;
            if (!this->load(protocol, path, output, option))
                TOD_RETURN_TRACE(false);
            return callback(output);
        }).detach();
        return true;
    }
    else
    {
        Data output;
        if (!this->load(protocol, path, output, option))
            TOD_RETURN_TRACE(false);
        return callback(output);
    }
}
    
    
//-----------------------------------------------------------------------------
bool FileSystem::load
(const String& uri, Data& output, const LoadOption& option) const
{
    String protocol, path;
    this->split_protocol_and_path(uri, protocol, path);
    return this->load(protocol, path, output, option);
}

    
//-----------------------------------------------------------------------------
bool FileSystem::load
(const String& protocol, const String& path,
 Data& output, const LoadOption& option) const
{
    //protocol 이 없으면 모든 protocol 에 요청해본다
    if (protocol.empty())
    {
        for (auto& p : this->protocols)
        {
            //Protocol 에 load를 요청
            if (!p->load(path, output, option))
                continue;
            
            return true;
        }
        
        //어떠한 Protocol 에도 요청을 처리하지 못했음
        TOD_RETURN_TRACE(false);
    }
    //protocol 이 있으면 해당 protocol 에 요청
    else
    {
        auto hash = protocol.hash();
        auto fl = std::find_if(
            this->protocols.begin(), this->protocols.end(),
            [hash](IProtocol* l) { return l->getProtocol().hash() == hash; });
        if (this->protocols.end() == fl)
            TOD_RETURN_TRACE(false);
        if (!(*fl)->load(path, output, option))
            TOD_RETURN_TRACE(false);
        return true;
    }
}


//-----------------------------------------------------------------------------
bool FileSystem::save(const String& uri, const char* data, uint64 size)
{
    String protocol, path;
    this->split_protocol_and_path(uri, protocol, path);
    
    //protocol 이 없으면 모든 protocol 에 요청해본다
    if (protocol.empty())
    {
        for (auto& p : this->protocols)
        {
            //Protocol 에 load를 요청
            if (!p->save(path, data, size))
                continue;
            
            return true;
        }
        
        //어떠한 Protocol 에도 요청을 처리하지 못했음
        TOD_RETURN_TRACE(false);
    }
    //protocol 이 있으면 해당 protocol 에 요청
    else
    {
        auto hash = protocol.hash();
        auto fl = std::find_if(
            this->protocols.begin(), this->protocols.end(),
            [hash](IProtocol* l) { return l->getProtocol().hash() == hash; });
        if (this->protocols.end() == fl)
            TOD_RETURN_TRACE(false);
        if (!(*fl)->save(path, data, size))
            TOD_RETURN_TRACE(false);
        return true;
    }
    return true;
}
    

//-----------------------------------------------------------------------------
void FileSystem::split_protocol_and_path
(const String& uri, String& protocol, String& path) const
{
    //protocol 을 파싱
    auto p = uri.find(S("://"));
    if (String::npos != p) protocol = std::move(uri.substr(0, p));
    
    //path 를 파싱
    path = std::move(uri.substr(p==String::npos?0:(p + 3), -1));
}
    

//-----------------------------------------------------------------------------
int64 FileSystem::getFileSize(const String& uri) const
{
    return 0;
}
    

//-----------------------------------------------------------------------------
String FileSystem::getCurrentWorkingDirectory() const
{
    char cwd[FILENAME_MAX];
    GetCurrentDir(cwd, sizeof(cwd));
    return cwd;
}


//-----------------------------------------------------------------------------
void FileSystem::reigster_protocol(IProtocol* Protocol)
{
    this->protocols.push_back(Protocol);
}

    
//-----------------------------------------------------------------------------
bool PosixProtocol::load
(const String& path, Data& output,
 const FileSystem::LoadOption& option)
{
    for (auto& spath : FileSystem::instance()->getSearchPaths())
    {
        String full_path;
        
        //절대경로라면
        if (path[0] == '/') full_path = path;
        else full_path = spath + S("/") + path;
        
        std::ifstream file(full_path.c_str(), std::ios::binary);
        if (!file) continue;
        
        //data buffer 준비
        file.seekg(0, file.end);
        output.resize(static_cast<size_t>(file.tellg()) + (option.isString() ? 1 : 0));
        if (option.isString()) output[output.size() - 1] = 0;
        file.seekg(0, file.beg);
        file.read(&output[0], output.size());
        file.close();
        
        return true;
    }
    
    TOD_LOG("error", S("file not found[%s]\n"), path.c_str());
    TOD_RETURN_TRACE(false);
}


//-----------------------------------------------------------------------------
bool PosixProtocol::save
(const String& path, const char* data, uint64 size)
{
    for (auto& spath : FileSystem::instance()->getSearchPaths())
    {
        String full_path;
        
        //절대경로라면
        if (path[0] == '/') full_path = path;
        else full_path = spath + S("/") + path;
        
        std::ofstream file(full_path.c_str(), std::ios::binary);
        if (!file) continue;
        file.write(data, size);
        file.close();
        
        return true;
    }
    
    TOD_THROW_EXCEPTION(S("file not written[%s]\n"), path.c_str());
    TOD_RETURN_TRACE(false);
}
    
}
