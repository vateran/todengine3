#pragma once
#include <D3D11.h>
#include "tod/graphics/inputlayout.h"
namespace tod::graphics
{

class DX11RenderInterface;
class DX11InputLayout : public InputLayout
{
public:
    DX11InputLayout(DX11RenderInterface* render_interface);
    virtual~DX11InputLayout();

    bool create(
          Shader* shader
        , const String& techniqueName
        , uint32 pass
        , const LayoutElements& layout_elements);
    void destroy() override;

    void use() override;

private:
    DX11RenderInterface* renderInterface;
    ID3D11InputLayout* d3dinputLayout;
};

}