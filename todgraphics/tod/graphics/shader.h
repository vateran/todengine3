#pragma once
#include "tod/object.h"
#include "tod/staticstring.h"
#include "tod/graphics/graphicsdef.h"
namespace tod::graphics
{

class Shader : public Derive<Shader, Object>
{
public:
    enum
    {
        SLOTCLASS_INPUT_PER_VERTEX_DATA,
        SLOTCLASS_INPUT_PER_INSTANCE_DATA,
    };

    struct LayoutElement
    {
        String semanticName;
        uint32 semanticIndex;
        Format format;
        uint32 inputSlotClass;
        uint32 instanceDataStepRate;
    };

    typedef std::vector<LayoutElement> LayoutElements;
    typedef std::list<std::pair<String, String>> Defines;

public:
    Shader() {}
    virtual~Shader() {}
    
    virtual bool create(const String& shader_fname, const Defines& defines) = 0;
    virtual void destroy() = 0;

    virtual bool begin(uint32 technique_index, uint32& passes) = 0;
    virtual void end() = 0;

    virtual bool beginPass(uint32 pass_index) = 0;
    virtual void endPass() = 0;
    
    virtual bool setParam(const StaticString& name, const std::any& value) = 0;
    virtual bool commitParams() = 0;

    virtual int32 getTechniqueIndex(const String& technique_name) = 0;
    virtual int32 getTechniquePassCount(uint32 technique_index) = 0;
};

}
