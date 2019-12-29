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


    //{"테크닉이름" :[{"name":"요소이름", "format" : "포멧"}, ...]}
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
            , "%@ 지정된 이름의 technique(%s)을 위한 InputLayout 을 찾을 수 없음"
            , technique_name.c_str());
        return nullptr;
    }

    return i->second;
}


//-----------------------------------------------------------------------------
bool InputLayoutCache::link(Shader* shader, const String& technique_name)
{
    //Config 가 정의되어 있는지 찾아본다
    auto i = this->layoutConfigs.find(technique_name);
    if (this->layoutConfigs.end() == i)
    {
        TOD_LOG("error"
            , "%@ technique name not found(%s)"
            , technique_name.c_str());
        TOD_RETURN_TRACE(false);
    }


    //이미 링크가 되어있는지 알아본다
    auto technique_name_hash = technique_name.hash();
    auto j = this->inputLayouts.find(technique_name_hash);
    if (this->inputLayouts.end() != j)
    {
        //이미 링크가 되어 있다면 걍 리턴
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
            , "%@ InputLayout 초기화 실패(%s)"
            , technique_name.c_str());
        TOD_SAFE_DELETE(input_layout);
        TOD_RETURN_TRACE(false);
    }

    this->inputLayouts.emplace(technique_name_hash, input_layout);

    return true;
}

}