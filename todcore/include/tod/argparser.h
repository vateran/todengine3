#pragma once
#include <unordered_map>
#include <functional>
#include "tod/any.h"
#include "tod/staticstring.h"
namespace tod
{

class ArgParser
{
private:
    enum class ArgType
    {
        Argument
        , Command
    };
    typedef std::function<bool()> CommandFunc;
    struct ArgInfo
    {
        ArgInfo(ArgType arg_type, const String& name
            , const String& desc, const CommandFunc& command_func)
            : argType(arg_type), name(name)
            , desc(desc), commandFunc(command_func) {}
        ArgInfo(ArgType arg_type, const String& name
            , const String& desc, const std::any& default_value)
            : argType(arg_type), name(name), desc(desc)
            , value(default_value), commandFunc(nullptr) {}
        ArgType argType;
        StaticString name;
        StaticString desc;
        std::any value;
        CommandFunc commandFunc;
    };
    typedef std::unordered_map<StaticString, ArgInfo*> ArgInfos;

public:
    ArgParser(const String& cmdline);
    ArgParser(int32 argc, char* argv[]);
    ~ArgParser();

    template <typename TYPE>
    void addOption(
          const String& name
        , const String& desc
        , const TYPE& default_value);
    void addCommand(
          const String& name
        , const String& desc
        , const CommandFunc& callback);

    template <typename TYPE>
    bool getOption(const String& name, TYPE& out) const;

    bool parse() const;
    bool execute() const;

    const String& getErrorMsg() const;

private:
    typedef std::vector<String> ArgStrings;
    ArgStrings argStrings;
    ArgInfos argInfos;
    mutable String errorMsg;
};


//-----------------------------------------------------------------------------
template <typename TYPE>
void ArgParser::addOption
(const String& name, const String& desc, const TYPE& default_value)
{
    TOD_ASSERT(
          this->argInfos.end() == this->argInfos.find(name)
        , "이미 같은 이름의 Command또는Option 이 있음 [%s]", name.c_str());
    this->argInfos.emplace(
          name
        , new ArgInfo(ArgType::Argument, name, desc, default_value));
}


//-----------------------------------------------------------------------------
template <typename TYPE>
bool ArgParser::getOption(const String& name, TYPE& out) const
{
    auto i = this->argInfos.find(name);
    if (this->argInfos.end() == i)
    {
        return false;
    }

    const ArgInfo* arg_info = i->second;
    out = std::any_cast<TYPE>(arg_info->value);

    return true;
}

}