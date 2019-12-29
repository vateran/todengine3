#pragma once
#include <d3dx11effect.h>
#include "tod/graphics/shader.h"
namespace tod::graphics
{

class DX11RenderInterface;
class DX11Shader : public Shader
{
public:
    DX11Shader(DX11RenderInterface* render_interface);
    virtual~DX11Shader();

    bool create(const String& shader_fname, const Defines& defines) override;
    void destroy() override;

    bool begin(uint32 technique_index, uint32& passes) override;
    void end() override;

    bool beginPass(uint32 pass_index) override;
    void endPass() override;

    int32 getTechniqueIndex(const String& technique_name) override;
    int32 getTechniquePassCount(uint32 technique_index) override;

    bool setParam(const StaticString& name, const std::any& value) override;

    bool commitParams() override;

    ID3DX11Effect* getEffect();

private:
    DX11RenderInterface* renderInterface;
    ID3DX11Effect* effect;
    ID3DX11EffectTechnique* currentTechnique;
    D3DX11_TECHNIQUE_DESC currentTechniqueDesc;
};

}