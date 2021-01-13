#include "tod/precompiled.h"
#include "tod/promise.h"
#include "tod/promise.inl"
#include "tod/Http.h"
#include <mutex>
#include <curl/curl.h>
namespace tod
{

    //-----------------------------------------------------------------------------
    class HttpInternal
    {
    public:
        HttpInternal()
        {
            curl_global_init(CURL_GLOBAL_DEFAULT);
        }
        ~HttpInternal()
        {
            curl_global_cleanup();
        }

        static HttpInternal* instance()
        {
            static HttpInternal s_instance;
            return &s_instance;
        }

        static int debug_trace
        (CURL* handle, curl_infotype type, char* data, size_t size, void* userp)
        {
            const char* text;
            (void)handle;
            (void)userp;

            switch (type) {
            case CURLINFO_TEXT:
                TOD_LOG("INFO", "== Info: %s", data);
            default:
                return 0;

            case CURLINFO_HEADER_OUT:
                text = "=> Send header";
                break;
            case CURLINFO_DATA_OUT:
                text = "=> Send data";
                break;
            case CURLINFO_SSL_DATA_OUT:
                text = "=> Send SSL data";
                break;
            case CURLINFO_HEADER_IN:
                text = "<= Recv header";
                break;
            case CURLINFO_DATA_IN:
                text = "<= Recv data";
                break;
            case CURLINFO_SSL_DATA_IN:
                text = "<= Recv SSL data";
                break;
            }

            TOD_LOG("INFO", "%s\n", text);
            return 0;
        }

    public:
        std::mutex m;
    };


//-----------------------------------------------------------------------------
Http::Response::Response()
: statusCode(0)
{
}


//-----------------------------------------------------------------------------
int32 Http::Response::getStatusCode() const
{
    return this->statusCode;
}


//-----------------------------------------------------------------------------
bool Http::Response::process(const String& url)
{
    CURL* curl = nullptr;
    {
        std::lock_guard<std::mutex> guard(HttpInternal::instance()->m);
        curl = curl_easy_init();
        if (nullptr == curl) TOD_RETURN_TRACE(false);
    }

    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Http::Response::write_data_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, LOW_SPEED_LIMIT);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, LOW_SPEED_TIME);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    //curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, Http::progress_callback);
    //curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, &response);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, nullptr);
    curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, HttpInternal::debug_trace);

    CURLcode code = CURLE_OK;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    code = curl_easy_perform(curl);
    if (CURLE_OK != code)
    {
        TOD_THROW_EXCEPTION(
            "@f(@l):curl_easy_perform: %s"
            , curl_easy_strerror(code));
        curl_easy_cleanup(curl);
        TOD_RETURN_TRACE(false);
    }

    this->statusCode = 0;
    code = curl_easy_getinfo(
        curl
        , CURLINFO_RESPONSE_CODE
        , &this->statusCode);
    if (code != CURLE_OK
        || !((this->statusCode >= 200)
          && (this->statusCode < 300)))
    {
        TOD_THROW_EXCEPTION(
            "@f(@l):Curl curl_easy_getinfo failed: %s"
            , curl_easy_strerror(code));
        curl_easy_cleanup(curl);
        TOD_RETURN_TRACE(false);
    }
    curl_easy_cleanup(curl);

    return true;
}


//-----------------------------------------------------------------------------
size_t Http::Response::write_data_callback
(void* ptr, size_t size, size_t nmemb, void* userdata)
{
    auto response = reinterpret_cast<Http::Response*>(userdata);
    size_t sizes = size * nmemb;
    response->data.insert(response->data.end(), (char*)ptr, (char*)ptr + sizes);
    return sizes;
}


//-----------------------------------------------------------------------------
/*int32 Http::Response::progress_callback
(void* userdata, double dltotal, double dlnow, double ultotal, double ulnow)
{
    auto self = reinterpret_cast<HttpResponse*>(userdata);
    self->totalSize = static_cast<int>(dltotal);
    return 0;
}*/


//-----------------------------------------------------------------------------
Http::Http()
: r(nullptr)
{
}

//-----------------------------------------------------------------------------
Http::~Http()
{
    TOD_SAFE_RELEASE(this->r);
}


//-----------------------------------------------------------------------------
typename Http::ResponsePromise& Http::request(const String& url)
{
    ResponsePromise* r = this->r = new ResponsePromise();
    std::thread([r, url]()
    {
        r->result.process(url);
        r->complete();
    }).detach();

    return *r;
}

}