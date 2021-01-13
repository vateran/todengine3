#pragma once
#include "tod/staticstring.h"
namespace tod
{

class File
{
public:
    //!@brief File::open 에서 open 옵션을 지정
    struct OpenMode
    {
        inline OpenMode& binary() { this->flags[BINARY] = true; return *this; }
        inline OpenMode& text() { this->flags[TEXT] = true; return *this; }
        inline OpenMode& append() { this->flags[APPEND] = true; return *this; }
        inline OpenMode& write() { this->flags[WRITE] = true; return *this; }
        inline OpenMode& read() { this->flags[READ] = true; return *this; }

        inline bool isBinary() const { return this->flags[BINARY]; }
        inline bool isText() const { return this->flags[TEXT]; }
        inline bool isAppend() const { return this->flags[APPEND]; }
        inline bool isWrite() const { return this->flags[WRITE]; }
        inline bool isRead() const { return this->flags[READ]; }

        const char* str() const;

    private:
        enum
        {
            WRITE
            , READ
            , APPEND
            , BINARY
            , TEXT
            , MAX
        };
        std::bitset<MAX> flags;
        mutable String modeStr;
    };

    enum SeekOrigin
    {
          CURRENT
        , START
        , END
    };

public:
    File();
    File(const char* fname, const OpenMode& open_mode);
    File(const String& fname, const OpenMode& open_mode);
    ~File();

    bool open(const char* fname, const OpenMode& open_mode);
    bool open(const String& fname, const OpenMode& open_mode);
    void close();

    bool valid() const;

    template <typename TYPE>
    uint32 write(const TYPE& value);
    uint32 write(const String& value);
    uint32 write(const StaticString& value);
    uint32 write(const void* data, uint32 size);

    template <typename TYPE>
    uint32 read(TYPE& value_out);
    uint32 read(String& value_out);
    uint32 read(void* data_out, uint32 size);

    void seek(int32 offset, SeekOrigin seek_origin = SeekOrigin::CURRENT);
    uint32 current();

private:
    FILE* fp;
    OpenMode openMode;
};

}

#include "tod/file.inl"