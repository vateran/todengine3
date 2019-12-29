#pragma once
#include "tod/component.h"
namespace tod::graphics
{

class Shader;
class InputLayout;

class ShaderComponent : public Derive<ShaderComponent, Component>
{
public:
    ShaderComponent();
    virtual~ShaderComponent();
    
    void begin(uint32& passes);
    void end();    
    bool setParam(const String& name, const std::any& value);
    bool commitParams();

    void setUri(const String& uri);
    const String& getUri();
    void setTechnique(const String& name);
    const String& getTechnique();

    Shader* getShader();
    
    static void bindProperty();
    
private:
    ObjRef<Shader> shader;
    String uri;
    String techniqueName;
    int32 techniqueIndex;
    InputLayout* inputLayout;
};

}
