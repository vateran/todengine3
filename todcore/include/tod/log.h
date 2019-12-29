#pragma once
#include <vector>
#include <memory>
#include <string>
#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif
namespace tod
{

class Log
{
public:
    virtual~Log() {}

    virtual void log(const char* log) const = 0;
};


class Logger
{
public:
    typedef std::vector<std::unique_ptr<Log>> CustomLogs;

    struct StandardLog
    {
        StandardLog& file(const char* log_name);
        StandardLog& console();

    private:
        enum
        {
            FILE,
            CONSOLE,
            MAX
        };
        std::bitset<MAX> flags;
        std::string logName;
        friend class Logger;
    };

public:
    Logger(int utc = 9);

    //!@brief Log 를 상속받은 커스텀 로그를 추가.
    ///추가한 Log 의 메모리는 내부에서 해제해준다
    void addCustomLog(Log* log);

    void setStandardLog(StandardLog log_type);

    void log(
          const char* tag
        , const char* file_name
        , const char* function
        , int line
        , const char* log, ...) const;

public:
    static Logger* instance()
    {
        static Logger s_instance;
        return &s_instance;
    }

private:
    int utc;
    CustomLogs customLogs;

};

#if defined (_DEBUG)
#define TOD_LOG(tag, msg, ...) { Logger::instance()->log(tag, __FILE__, __PRETTY_FUNCTION__, __LINE__, "@f(@l):[@q @u][@i]@p: " msg, __VA_ARGS__); }
#else
#define TOD_LOG(tag, msg, ...)
#endif

}