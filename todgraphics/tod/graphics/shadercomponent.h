#pragma once
#include "tod/component.h"
namespace tod::graphics
{
class Shader;
class Renderer;
class ShaderComponent : public Derive<ShaderComponent, Component>
{
public:
    ShaderComponent();
    virtual~ShaderComponent();
    
    void begin();
    void end();
    void setVShaderFileName(const String& fname);
    const String& getVShaderFileName();
    void setPShaderFileName(const String& fname);
    const String& getPShaderFileName();
    bool setParam(const String& name, const std::any& value);
    bool commitParams();
    
    static void bindProperty();
    
private:
    bool load();
    
private:
    String vshaderFileName;
    String pshaderFileName;
    ObjRef<Shader> shader;
    ObjRef<Renderer> renderer;
};

}
