#include "tod/graphics/inputlayoutcache.h"
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include "tod/filesystem.h"
#include "tod/graphics/renderer.h"
#include "tod/graphics/renderinterface.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
InputLayoutCache::InputLayoutCache()
{
}


//-----------------------------------------------------------------------------
InputLayoutCache::~InputLayoutCache()
{
    this->destroy();
}


//-----------------------------------------------------------------------------
bool InputLayoutCache::createFromConfig(const String& uri)
{
    Buffer buffer;
    if (!FileSystem::instance()->load(
          uri
        , &buffer
        , tod::FileSystem::LoadOption().string()))
    {
        TOD_RETURN_TRACE(false);
    }



    rapidjson::Document doc;
    if (doc
        .ParseInsitu<rapidjson::kParseInsituFlag>(buffer.data())
        .HasParseError())
    {
        TOD_LOG("error", "%@ %s(%zu): %s"
            , uri.c_str()
            , doc.GetErrorOffset()
            , rapidjson::GetParseError_En(doc.GetParseError()));
        TOD_RETURN_TRACE(false);
    }


    //{"��ũ���̸�" :[{"name":"����̸�", "format" : "����"}, ...]}
    for (auto i = doc.MemberBegin(); i != doc.MemberEnd(); ++i)
    {
        auto& config_values = i->value;
        InputLayout::LayoutElements* layoutElements
            = new InputLayout::LayoutElements();
        for (auto j = config_values.Begin()
            ; j != config_values.End()
            ; ++j)
        {
            auto& config_value = *j;
            auto& jformat = config_value["format"];

            FormatDesc* format_desc = FormatDesc::find(jformat.GetString());
            if (nullptr == format_desc)
            {
                TOD_LOG("error", "%@ [%s] format is not support"
                    , jformat.GetString());
                TOD_RETURN_TRACE(false);
            }

            layoutElements->push_back({
                config_value["name"].GetString()
                , format_desc->format });
        }

        this->layoutConfigs.emplace(
            i->name.GetString()
            , layoutElements);
    }

    return true;
}


//-----------------------------------------------------------------------------
void InputLayoutCache::destroy()
{
    for (auto i : this->layoutConfigs)
    {
        TOD_SAFE_DELETE(i.second);
    }
    this->layoutConfigs.clear();
}


//-----------------------------------------------------------------------------
InputLayout* InputLayoutCache::getInputLayout(const String& technique_name)
{
    auto i = this->inputLayouts.find(technique_name.hash());
    if (this->inputLayouts.end() == i)
    {
        TOD_LOG("error"
            , "%@ ������ �̸��� technique(%s)�� ���� InputLayout �� ã�� �� ����"
            , technique_name.c_str());
        return nullptr;
    }

    return i->second;
}


//-----------------------------------------------------------------------------
bool InputLayoutCache::link(Shader* shader, const String& technique_name)
{
    //Config �� ���ǵǾ� �ִ��� ã�ƺ���
    auto i = this->layoutConfigs.find(technique_name);
    if (this->layoutConfigs.end() == i)
    {
        TOD_LOG("error"
            , "%@ technique name not found(%s)"
            , technique_name.c_str());
        TOD_RETURN_TRACE(false);
    }


    //�̹� ��ũ�� �Ǿ��ִ��� �˾ƺ���
    auto technique_name_hash = technique_name.hash();
    auto j = this->inputLayouts.find(technique_name_hash);
    if (this->inputLayouts.end() != j)
    {
        //�̹� ��ũ�� �Ǿ� �ִٸ� �� ����
        TOD_LOG("warning"
            , "%@ technique already linked(%s)"
            , technique_name.c_str());
        return true;
    }


    InputLayout* input_layout = Renderer::instance()
        ->getRenderInterface()
        ->createInputLayout();
    if (!input_layout->create(shader, technique_name, 0, *(i->second)))
    {
        TOD_LOG("error"
            , "%@ InputLayout �ʱ�ȭ ����(%s)"
            , technique_name.c_str());
        TOD_SAFE_DELETE(input_layout);
        TOD_RETURN_TRACE(false);
    }

    this->inputLayouts.emplace(technique_name_hash, input_layout);

    return true;
}

}