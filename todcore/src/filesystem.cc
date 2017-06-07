#include <vector>
#include <thread>
#ifdef _WIN32
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
class PosixLoader : public FileSystem::ILoader
{
public:
    PosixLoader():ILoader(S("file")) {}
    
    bool load(const String& path,
              const LoadComplete& callback,
              const FileSystem::LoadOption& option)  override;
};
    
    
//-----------------------------------------------------------------------------
class ZipLoader : public FileSystem::ILoader
{
public:
    ZipLoader():ILoader(S("zip")) {}
    
    bool load(const String& path,
              const LoadComplete& callback,
              const FileSystem::LoadOption& option)  override
    {
        return false;
    }
};


//-----------------------------------------------------------------------------
class HttpLoader : public FileSystem::ILoader
{
public:
    enum
    {
        LOW_SPEED_LIMIT = 1L,
        LOW_SPEED_TIME = 5L,
    };
    
public:
    HttpLoader():ILoader(S("http")) {}
    
    bool load(const String& path,
              const LoadComplete& callback,
              const FileSystem::LoadOption& option) override
    {
        RequestData request_data;
        
        auto curl = curl_easy_init();
        if (!curl) return false;
        
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
                         HttpLoader::write_data_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &request_data);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, LOW_SPEED_LIMIT);
        curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, LOW_SPEED_TIME);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION,
                         HttpLoader::progress_callback);
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
            return false;
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
            return false;
        }
        curl_easy_cleanup(curl);
        
        if (option.isString())
            request_data.data.insert(request_data.data.end(), '0');
        callback(request_data.data);
        
        return true;
    }
    
private:
    struct RequestData
    {
        RequestData():
        responseCode(0)
        , recvBlockSize(0)
        , recvSize(0)
        , totalSize(0) {}
        
        Data data;
        int responseCode;
        int recvBlockSize;
        int recvSize;
        int totalSize;
    };
    static size_t write_data_callback
    (void *ptr, size_t size, size_t nmemb, void *userdata)
    {
        auto self = reinterpret_cast<RequestData*>(userdata);
        size_t sizes = size * nmemb;
        self->data.insert(self->data.end(), (char*)ptr, (char*)ptr+sizes);
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
    //Loader 들을 등록해준다
    this->reigster_loader(new PosixLoader);
    this->reigster_loader(new ZipLoader);
    this->reigster_loader(new HttpLoader);
    this->searchPaths.push_back(S("."));
}
    

//-----------------------------------------------------------------------------
FileSystem::~FileSystem()
{
    for (auto& fl : this->loaders) delete fl.second;
}


//-----------------------------------------------------------------------------
void FileSystem::setSearchPaths(const std::list<String>& paths)
{
    this->searchPaths = paths;
}

    
//-----------------------------------------------------------------------------
bool FileSystem::load
(const String& uri, const LoadComplete& callback,
 const LoadOption& option) const
{
    static const auto& make_exc_msg = [](const FileSystem* fs)
    {
        String exc_msg { S("protocol not registered or found (select : ") };
        bool f = false;
        for (auto& fl : fs->loaders)
        {
            if (!f) f = true; else exc_msg += S(",");
            exc_msg += fl.second->getProtocol().c_str();
            exc_msg += S("://");
        }
        exc_msg += S(")");
        TOD_THROW_EXCEPTION(exc_msg.c_str());
    };
    
    
    //protocol 을 파싱
    auto p = uri.find(S("://"));
    String protocol { uri.substr(0, p) };
    if (String::npos == p) protocol = S("file");
    else protocol = uri.substr(0, p);
    

    //지정된 protocol 의 Loader 를 찾아본다
    auto fl = this->loaders.find(protocol.hash());
    if (this->loaders.end() == fl)
    {
        make_exc_msg(this);
        return false;
    }
    
    String path { uri.substr(p==String::npos?0:(p + 3), -1) };
    auto process = [this, fl, path, callback, option]()
    {
        //Loader 에 load를 요청
        if (!fl->second->load(path, callback, option))
            return false;
        return true;
    };
    
    if (option.isAsync())
    {
        std::thread(process).detach();
        return true;
    }
    else return process();
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
void FileSystem::reigster_loader(ILoader* loader)
{
    this->loaders.insert(std::make_pair(loader->getProtocol().hash(), loader));
}

    
//-----------------------------------------------------------------------------
bool PosixLoader::load
(const String& path, const LoadComplete& callback,
 const FileSystem::LoadOption& option)
{
    for (auto& spath : FileSystem::instance()->getSearchPaths())
    {
        String full_path { spath + S("/") + path };
        FILE* fp = fopen(full_path.c_str(), "rb");
        if (nullptr == fp) continue;
        
        //data buffer 준비
        Data data;
        fseek(fp, 0, SEEK_END);
        data.resize(ftell(fp) + (option.isString()?1:0));
        if (option.isString()) data[data.size() - 1] = 0;
        fseek(fp, 0, SEEK_SET);
        fread(&data[0], data.size(), 1, fp);
        
        fclose(fp);
        
        callback(data);
        
        return true;
    }
    
    TOD_THROW_EXCEPTION(S("file not found[%s]"), path.c_str());
    return false;
}
    
}
