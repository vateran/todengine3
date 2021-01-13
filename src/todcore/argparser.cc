#include "tod/precompiled.h"
#include "tod/argparser.h"
namespace tod
{

//-----------------------------------------------------------------------------
ArgParser::ArgParser(const String& cmdline)
{
    cmdline.split(S(" "), this->argStrings);
}


//-----------------------------------------------------------------------------
ArgParser::ArgParser(int32 argc, char* argv[])
{
    for (int32 i = 0; i < argc; ++i)
    {
        this->argStrings.emplace_back(argv[i]);
    }
}


//-----------------------------------------------------------------------------
ArgParser::~ArgParser()
{
    for (auto i : this->argInfos)
    {
        TOD_SAFE_DELETE(i.second);
    }
}


//-----------------------------------------------------------------------------
void ArgParser::addCommand
(const String& name, const String& desc, const CommandFunc& callback)
{
    TOD_ASSERT(
        this->argInfos.end() == this->argInfos.find(name)
        , "�̹� ���� �̸��� Command�Ǵ�Option �� ���� [%s]", name.c_str());
    this->argInfos.emplace(
          name
        , new ArgInfo(ArgType::Command, name, desc, callback));
}


//-----------------------------------------------------------------------------
bool ArgParser::parse() const
{
    for (size_t i = 1; i < this->argStrings.size();)
    {
        const String& str = this->argStrings[i];
        size_t det = str.find(S("--"));
        if (String::npos != det)
        {
            //1. arg name �� �д´�
            String name = str.substr(det + 2, -1);
            auto ai = this->argInfos.find(name);
            if (this->argInfos.end() == ai)
            {
                this->errorMsg = String::fromFormat(
                      "�������� �ʴ� �ɼ�[%s]"
                    , name.c_str());
                return false;
            }

            ++i;

            const ArgInfo* arg_info = ai->second;
            if (ArgType::Argument != arg_info->argType)
            {
                continue;
            }

            //2. arg value �� �д´�
            const String& value = this->argStrings[i];
            if (arg_info->value.type().name() == typeid(bool).name())
            {
                //bool �� Ư���ϰ� argument �������ε� true ��
                //���࿡ value �� ������ -- �� value ���� find �Ǹ�
                //argument ���� �Ѱ���
                if (String::npos != value.find(S("--")))
                {
                    ai->second->value = true;
                    continue;
                }
                else
                {
                    ai->second->value = (value == "true");
                }
            }
            else if (arg_info->value.type().name() == typeid(int32).name())
            {
                ai->second->value = String::atoi(value.c_str());
            }
            else if (arg_info->value.type().name() == typeid(int64).name())
            {
                ai->second->value = String::atoi64(value.c_str());
            }
            else if (arg_info->value.type().name() == typeid(float).name())
            {
                ai->second->value = String::atof<float>(value.c_str());
            }
            else if (arg_info->value.type().name() == typeid(double).name())
            {
                ai->second->value = String::atof<double>(value.c_str());
            }
            else if (arg_info->value.type().name() == typeid(String).name())
            {
                ai->second->value = value;
            }
            else
            {
                TOD_ASSERT(
                      false
                    , "�������� �ʴ� ���� [%s]"
                    , arg_info->value.type().name());
                this->errorMsg = String::fromFormat(
                      "�������� �ʴ� ���� [%s]"
                    , arg_info->value.type().name());
                return false;
            }

            ++i;
        }
        else
        {
            this->errorMsg = String::fromFormat(
                  "Option ������ -- �� ���Ծ���[%s]"
                , str.c_str());
            return false;
        }
    }

    return true;
}


//-----------------------------------------------------------------------------
bool ArgParser::execute() const
{
    for (size_t i = 1; i < this->argStrings.size(); ++i)
    {
        const String& str = this->argStrings[i];
        size_t det = str.find(S("--"));
        if (String::npos != det)
        {
            //1. command name �� �д´�
            String name = str.substr(det + 2, -1);
            auto ai = this->argInfos.find(name);
            if (this->argInfos.end() == ai)
            {
                this->errorMsg = String::fromFormat(
                      "�������� �ʴ� ���[%s]"
                    , name.c_str());
                return false;
            }

            const ArgInfo* arg_info = ai->second;
            if (ArgType::Command == arg_info->argType)
            {
                //2. command �� �����Ѵ�
                if (nullptr != arg_info->commandFunc)
                {
                    if (false == arg_info->commandFunc())
                    {
                        this->errorMsg = String::fromFormat(
                              "��� ���࿡ �����Ͽ���[%s]"
                            , name.c_str());
                        return false;
                    }
                }
            }
        }
    }

    return true;
}


//-----------------------------------------------------------------------------
const String& ArgParser::getErrorMsg() const
{
    return this->errorMsg;
}

}