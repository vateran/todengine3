#include "tod/precompiled.h"
#include "tod/log.h"
#include <stdarg.h>
#if defined (TOD_PLATFORM_WINDOWS)
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define TOD_ALLOCA(size) _malloca(size)
#define TOD_FREEA(p) _freea(p);
#else
#define TOD_ALLOCA(size) alloca(size)
#define tOD_FTOD_FREEAREEA(p) 
#endif

namespace tod
{


//-----------------------------------------------------------------------------
class ConsoleLog : public Log
{
public:
    virtual~ConsoleLog() {}

    virtual void log(const char* log) const
    {
#if defined (TOD_PLATFORM_WINDOWS)
        OutputDebugStringA(log);
        OutputDebugStringA("\n");
#else
        printf(log);
#endif
    }
};


//-----------------------------------------------------------------------------
class FileLog : public Log
{
public:
    FileLog(const std::string& log_name)
    {
        std::string log_file_name;
        time_t rawtime;
        time(&rawtime);
        struct tm* ptm = localtime(&rawtime);
        char time_buf[32];
        TOD_SNPRINTF(
            time_buf
            , sizeof(time_buf)
            , "_(%02d-%02d-%02d)_(%02d-%02d-%02d)"
            , ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday
            , ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
        log_file_name += log_name;
        log_file_name += time_buf;
        log_file_name += ".log";

#if defined (TOD_PLATFORM_WINDOWS)
        fopen_s(&this->fp, log_file_name.c_str(), "wb");
#else
        this->fp = fopen(log_file_name.c_str(), "wb");
#endif
    }
    virtual~FileLog()
    {
        if (nullptr != this->fp)
        {
            fclose(this->fp);
        }
    }

    virtual void log(const char* log) const
    {
        fprintf(this->fp, "%s\n", log);
    }

private:
    FILE* fp;
};


//-----------------------------------------------------------------------------
Logger::Logger(int utc)
: utc(utc)
{

}


//-----------------------------------------------------------------------------
void Logger::addCustomLog(Log* log)
{
    this->customLogs.emplace_back(log);
}


//-----------------------------------------------------------------------------
void Logger::setStandardLog(StandardLog log_type)
{
    if (true == log_type.flags[StandardLog::CONSOLE])
    {
        this->addCustomLog(new ConsoleLog);
    }

    if (true == log_type.flags[StandardLog::FILE])
    {
        this->addCustomLog(new FileLog(log_type.logName));
    }
}


//-----------------------------------------------------------------------------
void Logger::log(
   const char* tag, const char* file_name, const char* function
 , int line, const char* log, ...) const
{
    //일반적인 formatting 을 수행
    va_list args1;
    va_start(args1, log);
    va_list args2;
    va_copy(args2, args1);
    int log_buffer_len = 1 + std::vsnprintf(nullptr, 0, log, args1);
    char* log_buffer = static_cast<char*>(TOD_ALLOCA(log_buffer_len));
    va_end(args1);
    std::vsnprintf(log_buffer, log_buffer_len, log, args2);
    va_end(args2);
    
    //TOD Engine 고유 @ 포맷팅 수행
    thread_local static String out_log_buffer;
    out_log_buffer.clear();
    for (int i = 0; i < log_buffer_len - 1;)
    {
        if ('@' == log_buffer[i])
        {
            char directive = log_buffer[++i];
            switch (directive)
            {
            // Function
            case 'p':
                out_log_buffer += function;
                break;
            // Tag
            case 'i':
                out_log_buffer += tag;
                break;
            // FileName
            case 'f':
                out_log_buffer += file_name;
                break;
            // Line Number
            case 'l':
                {
                    char line_buf[16];
                    TOD_SNPRINTF(line_buf, sizeof(line_buf), "%d", line);
                    out_log_buffer += line_buf;
                }
                break;
            //Local Date
            case 'd':
                {
                    time_t rawtime;
                    time(&rawtime);
                    struct tm* ptm = localtime(&rawtime);
                    char time_buf[16];
                    TOD_SNPRINTF(
                        time_buf
                        , sizeof(time_buf)
                        , "%02d/%02d/%02d"
                        , ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
                    out_log_buffer += time_buf;
                }
                break;
            //UTC Date
            case 'q':
                {
                    time_t rawtime;
                    time(&rawtime);
                    struct tm* ptm = gmtime(&rawtime);
                    char time_buf[16];
                    TOD_SNPRINTF(
                        time_buf
                        , sizeof(time_buf)
                        , "%02d/%02d/%02d"
                        , ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday);
                    out_log_buffer += time_buf;
                }
            break;
            //LocalTimeStamp
            case 't':
                {                    
                    time_t rawtime;
                    time(&rawtime);
                    struct tm* ptm = localtime(&rawtime);
                    char time_buf[16];
                    TOD_SNPRINTF(
                        time_buf
                        , sizeof(time_buf)
                        , "%02d:%02d:%02d"
                        , ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
                    out_log_buffer += time_buf;
                }
                break;
            //UTCTimeStamp
            case 'u':
                {
                    time_t rawtime;
                    time(&rawtime);
                    struct tm* ptm = gmtime(&rawtime);
                    char time_buf[16];
                    TOD_SNPRINTF(
                        time_buf
                        , sizeof(time_buf)
                        , "%02d:%02d:%02d"
                        , (ptm->tm_hour + this->utc) % 24
                        , ptm->tm_min, ptm->tm_sec);
                    out_log_buffer += time_buf;
                }
            break;
            default:
                out_log_buffer.push_back('@');
                out_log_buffer.push_back(directive);
                break;
            }

            ++i;
        }
        else
        {
            out_log_buffer += log_buffer[i++];
        }
    }
        
    for (auto& log : this->customLogs)
    {
        log->log(out_log_buffer.c_str());
    }

    TOD_FREEA(log_buffer);
}


//-----------------------------------------------------------------------------
Logger::StandardLog& Logger::StandardLog::file(const char* log_name)
{
    this->logName = log_name;
    this->flags[FILE] = true; return *this;
}


//-----------------------------------------------------------------------------
Logger::StandardLog& Logger::StandardLog::console()
{
    this->flags[CONSOLE] = true;
    return *this;
}

}