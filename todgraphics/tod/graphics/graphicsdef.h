#pragma once
#include <unordered_map>
#include "tod/basetype.h"
#include "tod/staticstring.h"
namespace tod::graphics
{

    //-----------------------------------------------------------------------------
#define ENUM_DESC_FUNCTIONALITY(ENUM, ENUM_DESC) \
    static ENUM_DESC repository[static_cast<uint32>(ENUM::MAX)];\
    typedef std::unordered_map<StaticString, ENUM_DESC*> Indexer;\
    static ENUM_DESC* find(const StaticString& name) {\
        static Indexer indexer;\
        if (true == indexer.empty()) {\
            uint32 count = static_cast<uint32>(ENUM::MAX);\
            for (uint32 i = 0; i < count; ++i) {\
                ENUM_DESC* enum_desc = &ENUM_DESC::repository[i];\
                indexer.emplace(enum_desc->name, enum_desc); }}\
        auto i = indexer.find(name);\
        if (indexer.end() == i) { return nullptr; }\
        return i->second; }


//-----------------------------------------------------------------------------
enum class Format : uint8
{
      NONE
    , L8F
    , R8G8B8A8
    , R8G8B8A8U
    , R8G8B8A8UN
    , R8G8B8A8UN_SRGB
    , R16G16B16A16F
    , R32F
    , R32G32F
    , R32G32B32F
    , R32G32B32A32F
    , DEPTH16UN
    , DEPTH32F
    
    , MAX
};


//-----------------------------------------------------------------------------
struct FormatDesc
{
    StaticString name;
    Format format;
    uint32 stride;

    ENUM_DESC_FUNCTIONALITY(Format, FormatDesc)
};


//-----------------------------------------------------------------------------
enum class TextureWrap : uint8
{
      REPEAT
    , MIRRORED_REPEAT
    , CLAMP_TO_EDGE
    , CLAMP_TO_BORDER
};


//-----------------------------------------------------------------------------
enum class TextureFilter : uint8
{
      NEAREST
    , LINEAR
    , BILINEAR
    , TRILINEAR
    , ANISOTROPIC
};


//-----------------------------------------------------------------------------
enum class TextureType : uint8
{
      DIFFUSE
    , NORMAL
    , SPECULAR
    , LIGHT
    , BUMP
    , OPACITY
    , EMISSIVE
    , AMBIENT
    , SHININESS
    , DISPLACEMENT
    , REFLECTION
    , HEIGHT

    , MAX
};


//-----------------------------------------------------------------------------
struct TextureTypeDesc
{
    StaticString name;
    StaticString shaderParamName;
    TextureType type;

    ENUM_DESC_FUNCTIONALITY(TextureType, TextureTypeDesc)
};

}
