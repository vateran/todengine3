#pragma once
#include "tod/string.h"
#include "tod/promise.h"
#include "tod/buffer.h"
namespace tod
{

class Http
{
public:
    enum
    {
        LOW_SPEED_LIMIT = 1L,
        LOW_SPEED_TIME = 5L,
    };

public:
    class Response
    {
    public:
        Response();
        int32 getStatusCode() const;

    private:
        bool process(const String& url);
        static size_t write_data_callback
        (void* ptr, size_t size, size_t nmemb, void* userdata);

    private:
        Buffer data;
        int32 statusCode;
        friend Http;
    };

    typedef Promise<Response, Http> ResponsePromise;

public:
    Http();
    ~Http();

    ResponsePromise& request(const String& url);

private:
    ResponsePromise* r;
};

}