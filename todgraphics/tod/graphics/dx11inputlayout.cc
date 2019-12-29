#include "tod/graphics/dx11inputlayout.h"
#include "tod/graphics/dx11renderinterface.h"
#include "tod/graphics/dx11shader.h"
#include "tod/exception.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
DX11InputLayout::DX11InputLayout(DX11RenderInterface* render_interface)
: renderInterface(render_interface)
{
}


//-----------------------------------------------------------------------------
DX11InputLayout::~DX11InputLayout()
{
    this->destroy();
}


//-----------------------------------------------------------------------------
bool DX11InputLayout::create
(Shader* shader, const String& technique_name,
 uint32 pass, const LayoutElements& layout_elements)
{
    //D3D11_INPUT_ELEMENT_DESC 를 구성한다
    std::vector<D3D11_INPUT_ELEMENT_DESC> descs;
    uint32 offset = 0;
    for (auto& elem : layout_elements)
    {
        descs.push_back({
            elem.semanticName.c_str()
            , 0
            , DX11RenderInterface::FormatToDXGI_FORMAT(elem.format)
            , 0
            , offset
            , D3D11_INPUT_PER_VERTEX_DATA
            , 0 });

        offset += FormatDesc::repository[static_cast<uint32>(elem.format)].stride;
    }


    //ID3D11InputLayout 생성
    DX11Shader* dx11shader = static_cast<DX11Shader*>(shader);
    auto technique = dx11shader
        ->getEffect()
        ->GetTechniqueByName(technique_name.c_str());
    if (nullptr == technique) TOD_RETURN_TRACE(false);

    D3DX11_TECHNIQUE_DESC desc;
    HRESULT hr = technique->GetDesc(&desc);
    if (FAILED(hr))
    {
        TOD_LOG("error", "%@ HRESULT(%d)", hr);
        TOD_RETURN_TRACE(false);
    }

    D3DX11_PASS_DESC pass_desc;
    hr = technique->GetPassByIndex(pass)->GetDesc(&pass_desc);
    if (FAILED(hr))
    {
        TOD_LOG("error", "%@ HRESULT(%d)", hr);
        TOD_RETURN_TRACE(false);
    }

    hr = this->renderInterface
        ->getD3D11Device()
        ->CreateInputLayout(
            descs.data()
            , static_cast<UINT>(descs.size())
            , pass_desc.pIAInputSignature
            , pass_desc.IAInputSignatureSize
            , &this->d3dinputLayout);
    if (FAILED(hr))
    {
        TOD_LOG("error", "%@ HRESULT(%d)", hr);
        TOD_RETURN_TRACE(false);
    }

    return true;
}


//-----------------------------------------------------------------------------
void DX11InputLayout::destroy()
{
    TOD_SAFE_COM_RELEASE(this->d3dinputLayout);
}


//-----------------------------------------------------------------------------
void DX11InputLayout::use()
{
    TOD_ASSERT(nullptr != this->d3dinputLayout, "초기화 안됨");

    this->renderInterface
        ->getD3D11DeviceContext()
        ->IASetInputLayout(this->d3dinputLayout);
}

};