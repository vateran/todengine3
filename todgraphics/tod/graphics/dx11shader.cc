#include "tod/graphics/dx11shader.h"
#include "tod/buffer.h"
#include "tod/filesystem.h"
#include "tod/graphics/inputlayoutcache.h"
#include "tod/graphics/dx11renderinterface.h"
#include "tod/graphics/dx11constantbuffer.h"
#include "tod/graphics/dx11texture.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
class D3DIncludeImpl : public ID3DInclude
{
public:
    STDMETHOD(Open)(
        D3D_INCLUDE_TYPE IncludeType
        , LPCSTR pFileName
        , LPCVOID pParentData
        , LPCVOID* ppData
        , UINT* pBytes)
    {
        return S_OK;
    }

    STDMETHOD(Close)(LPCVOID pData)
    {
        return S_OK;
    }

    static D3DIncludeImpl s_instance;
};

D3DIncludeImpl D3DIncludeImpl::s_instance;


//-----------------------------------------------------------------------------
DX11Shader::DX11Shader(DX11RenderInterface* render_interface)
: renderInterface(render_interface)
, effect(nullptr)
, currentTechnique(nullptr)
{
}


//-----------------------------------------------------------------------------
DX11Shader::~DX11Shader()
{
    this->destroy();
}


//-----------------------------------------------------------------------------
bool DX11Shader::create(const String& shader_fname, const Defines& defines)
{
    std::vector<D3D_SHADER_MACRO> shaderMacros;
    {
        shaderMacros.resize(defines.size());
        for (auto& define : defines)
        {
            D3D_SHADER_MACRO shaderMacro;
            shaderMacro.Name = define.first.c_str();
            shaderMacro.Definition = define.second.c_str();
            shaderMacros.emplace_back(shaderMacro);
        }
        D3D_SHADER_MACRO shaderMacro;
        shaderMacro.Name = nullptr;
        shaderMacro.Definition = nullptr;
        shaderMacros.emplace_back(shaderMacro);
    }

    ID3D10Blob* errorMessage = nullptr;
    ID3D10Blob* vsBuffer = nullptr;
    Buffer shader_data;
    FileSystem::instance()->load(shader_fname, &shader_data);
    if (FAILED(D3DX11CompileEffectFromMemory(
        shader_data.data()
        , shader_data.size()
        , shader_fname.c_str()
        , &shaderMacros[0]
        , &D3DIncludeImpl::s_instance
        , 0
        , 0
        , renderInterface->getD3D11Device()
        , &this->effect
        , &errorMessage)))
    {
        TOD_LOG("error"
            , "%@ shader load error(%s): %s\n"
            , shader_fname.c_str()
            , static_cast<char*>(errorMessage->GetBufferPointer()));
        TOD_SAFE_COM_RELEASE(errorMessage);
        TOD_RETURN_TRACE(false);
    }

    D3DX11_EFFECT_DESC effect_desc;
    if (FAILED(this->effect->GetDesc(&effect_desc)))
    {
        TOD_LOG("error"
            , "DX11Shader::load(%s): %s\n"
            , shader_fname.c_str()
            , static_cast<char*>(errorMessage->GetBufferPointer()));
        TOD_RETURN_TRACE(false);
    }

    for (uint32 i = 0; i < effect_desc.Techniques; ++i)
    {
        auto technique = this->effect->GetTechniqueByIndex(i);

        D3DX11_TECHNIQUE_DESC desc;
        HRESULT hr = technique->GetDesc(&desc);
        if (FAILED(hr))
        {
            TOD_LOG("error", "%@ HRESULT(%d)", hr);
            TOD_RETURN_TRACE(false);
        }

        if (!InputLayoutCache::instance()->link(this, desc.Name))
        {
            TOD_LOG("error"
                , "%@ InputLayout link error(technique_name:%s)"
                , desc.Name);
            TOD_RETURN_TRACE(false);
        }
    }

    return true;
}


//-----------------------------------------------------------------------------
void DX11Shader::destroy()
{
    TOD_SAFE_COM_RELEASE(this->effect);
}


//-----------------------------------------------------------------------------
bool DX11Shader::begin(uint32 technique_index, uint32& passes)
{
    this->currentTechnique = this->effect->GetTechniqueByIndex(technique_index);
    if (FAILED(this->currentTechnique->GetDesc(&this->currentTechniqueDesc)))
    {
        TOD_RETURN_TRACE(false);
    }
    passes = this->currentTechniqueDesc.Passes;
    return (nullptr != this->currentTechnique);
}


//-----------------------------------------------------------------------------
void DX11Shader::end()
{
    TOD_SAFE_COM_RELEASE(this->currentTechnique);
}


//-----------------------------------------------------------------------------
bool DX11Shader::beginPass(uint32 pass_index)
{
    auto pass = this->currentTechnique->GetPassByIndex(pass_index);
    pass->Apply(0, this->renderInterface->getD3D11DeviceContext());
    TOD_SAFE_COM_RELEASE(pass);
    return true;
}


//-----------------------------------------------------------------------------
void DX11Shader::endPass()
{
}


//-----------------------------------------------------------------------------
int32 DX11Shader::getTechniqueIndex(const String& technique_name)
{
    TOD_ASSERT(nullptr != this->effect, "초기화 필요");

    D3DX11_EFFECT_DESC effect_desc;
    HRESULT hr = this->effect->GetDesc(&effect_desc);
    if (FAILED(hr))
    {
        TOD_LOG("error", "%@ HRESULT(%d)", hr);
        TOD_RETURN_TRACE(-1);
    }

    for (uint32 i = 0; i < effect_desc.Techniques; ++i)
    {
        auto technique = this->effect->GetTechniqueByIndex(i);

        D3DX11_TECHNIQUE_DESC desc;
        hr = technique->GetDesc(&desc);
        if (FAILED(hr))
        {
            TOD_LOG("error", "%@ HRESULT(%d)", hr);
            TOD_RETURN_TRACE(-1);
        }

        if (strcmp(desc.Name, technique_name.c_str()) == 0)
        {
            return i;
        }
    }

    TOD_LOG("error"
        , "%@ technique_name(%s) 찾을 수 없음"
        , technique_name.c_str());
    TOD_RETURN_TRACE(-1);
}


//-----------------------------------------------------------------------------
int32 DX11Shader::getTechniquePassCount(uint32 technique_index)
{
    TOD_ASSERT(nullptr != this->effect, "초기화 필요");

    auto technique = this->effect->GetTechniqueByIndex(technique_index);
    if (nullptr == technique)
    {
        TOD_LOG("error"
            , "%@ technique_index(%u) 찾을 수 없음"
            , technique_index);
        TOD_RETURN_TRACE(-1);
    }

    D3DX11_TECHNIQUE_DESC desc;
    HRESULT hr = technique->GetDesc(&desc);
    if (FAILED(hr))
    {
        TOD_LOG("error", "%@ HRESULT(%d)", hr);
        TOD_RETURN_TRACE(-1);
    }

    return desc.Passes;
}


//-----------------------------------------------------------------------------
bool DX11Shader::setParam(const StaticString& name, const std::any& value)
{
    if (typeid(Texture*) == value.type())
    {
        auto var = this->effect->GetVariableByName(name.c_str());
        if (false == var->IsValid())
        {
            TOD_LOG("error"
                , "[%s] 이름의 Shader 변수는 존재하지 않습니다."
                , name.c_str());
            TOD_RETURN_TRACE(false);
        }

        auto res = var->AsShaderResource();
        auto texture = std::any_cast<Texture*>(value);
        auto d3dtexutre = static_cast<DX11Texture*>(texture);
        if (FAILED(res->SetResource(d3dtexutre->getSRV())))
        {
            TOD_RETURN_TRACE(false);
        }
    }
    else if (typeid(ConstantBuffer*) == value.type())
    {
        auto var = this->effect->GetConstantBufferByName(name.c_str());
        if (false == var->IsValid())
        {
            TOD_LOG("error"
                , "[%s] 이름의 Shader ConstantBuffer는 존재하지 않습니다."
                , name.c_str());
            TOD_RETURN_TRACE(false);
        }

        auto cbuffer = std::any_cast<ConstantBuffer*>(value);
        auto d3dcbuffer = static_cast<DX11ConstantBuffer*>(cbuffer);
        if (FAILED(var->SetConstantBuffer(d3dcbuffer->getD3D11Buffer())))
        {
            TOD_RETURN_TRACE(false);
        }
    }
    else
    {
        TOD_ASSERT(false, "지원되지 않는 형식입니다.");
    }

    return true;
}


//-----------------------------------------------------------------------------
bool DX11Shader::commitParams()
{
    return true;
}


//-----------------------------------------------------------------------------
ID3DX11Effect* DX11Shader::getEffect()
{
    return this->effect;
}

}