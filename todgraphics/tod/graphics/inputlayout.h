#pragma once
#include <array>
#include "tod/object.h"
#include "tod/graphics/vector2.h"
#include "tod/graphics/vector3.h"
#include "tod/graphics/vector3.h"
#include "tod/graphics/graphicsdef.h"
#include "tod/graphics/compactfloat.h"
namespace tod::graphics
{

class Shader;


struct NormalVertexLayout
{
    tod::graphics::Vector3 position;
    tod::graphics::Vector3 normal;
    tod::graphics::Vector2 tex;
};


#define TOD_BONE_INDEX_COUNT 4
struct SkinnedVertexLayout
{
    tod::graphics::Vector3 position;
    tod::graphics::Vector3 normal;
    tod::graphics::Vector2 tex;
    std::array<uint8, TOD_BONE_INDEX_COUNT> boneIndex;
    std::array<CompactFloat<uint8>, TOD_BONE_INDEX_COUNT> weight;
};



class InputLayout : public Derive<InputLayout, Object>
{
public:
    struct LayoutElement
    {
        String semanticName;
        Format format;
    };
    typedef std::vector<LayoutElement> LayoutElements;

public:
    virtual~InputLayout() {}

    virtual bool create(
          Shader* shader
        , const String& techniqueName
        , uint32 pass
        , const LayoutElements& layout_elements) = 0;
    virtual void destroy() = 0;

    virtual void use() = 0;
};

}