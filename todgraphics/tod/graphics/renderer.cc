#include "tod/graphics/renderer.h"
#include "tod/graphics/renderinterface.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
Renderer::Renderer()
: renderInterface(nullptr)
{

}


//-----------------------------------------------------------------------------
Renderer::~Renderer()
{
    this->destroy();
}


//-----------------------------------------------------------------------------
bool Renderer::create(RenderInterfaceType render_interface_type)
{
    switch (render_interface_type)
    {
    case RenderInterfaceType::Direct3D11:
        //this->renderInterface = new DX11RenderInterface();
        break;
    case RenderInterfaceType::OpenGL:
        break;
    default:
        TOD_RETURN_TRACE(false);
    }

    //if (false == this->renderInterface->open(window_handle, ))
    {
        TOD_RETURN_TRACE(false);
    }

    return true;
}



//-----------------------------------------------------------------------------
void Renderer::destroy()
{
    TOD_SAFE_DELETE(this->renderInterface);
}


//-----------------------------------------------------------------------------
bool Renderer::isValid()
{
    return nullptr != this->renderInterface;
}


//-----------------------------------------------------------------------------
RenderInterface* Renderer::getRenderInterface()
{
    return this->renderInterface;
}

}