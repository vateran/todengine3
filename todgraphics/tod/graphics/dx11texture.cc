#include "tod/graphics/dx11texture.h"
#include "tod/graphics/dx11renderinterface.h"
#include <DirectXMath.h>
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include "tod/buffer.h"
#include "tod/filesystem.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
DX11Texture::DX11Texture(DX11RenderInterface* render_interface)
: renderInterface(render_interface)
, d3dtexture2D(nullptr)
, d3dtextureSRV(nullptr)
{
}


//-----------------------------------------------------------------------------
DX11Texture::~DX11Texture()
{
    this->destroy();
}


//-----------------------------------------------------------------------------
bool DX11Texture::create(int32 width, int32 height, Format format, void* data)
{
    return true;
}


//-----------------------------------------------------------------------------
void DX11Texture::destroy()
{
    TOD_SAFE_COM_RELEASE(this->d3dtextureSRV);
    TOD_SAFE_COM_RELEASE(this->d3dtexture2D);
}


//-----------------------------------------------------------------------------
bool DX11Texture::load(const String& uri, uint8 mipmap_level)
{
    Buffer data;
    if (false == FileSystem::instance()->load(uri, &data))
    {
        TOD_RETURN_TRACE(false);
    }

    String file_extension = uri.extractFileExtension();
    if (S("dds") == file_extension)
    {
        if (FAILED(DirectX::CreateDDSTextureFromMemoryEx(
            this->renderInterface->getD3D11Device()
            , reinterpret_cast<const uint8_t*>(data.data())
            , data.size()
            , data.size()
            , D3D11_USAGE_DEFAULT
            , D3D11_BIND_SHADER_RESOURCE
            , 0, 0, true
            , &this->d3dtexture2D
            , &this->d3dtextureSRV)))
        {
            TOD_RETURN_TRACE(false);
        }

        ID3D11Texture2D* texture = nullptr;
        this->d3dtexture2D->QueryInterface(IID_ID3D11Texture2D, (void**)&texture);

        D3D11_TEXTURE2D_DESC desc;
        texture->GetDesc(&desc);
        
        this->width = desc.Width;
        this->height = desc.Height;
        this->format = DX11RenderInterface::DXGI_FORMATToFormat(desc.Format);
    }
    else
    {
        ilInit();
        ilLoadL(0, data.data(), static_cast<ILuint>(data.size()));
        ilGetInteger(IL_IMAGE_FORMAT);
        ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

        if (S("tga") != file_extension)
        {
            iluFlipImage();
        }

        D3D11_TEXTURE2D_DESC desc;
        desc.Width = ilGetInteger(IL_IMAGE_WIDTH);
        desc.Height = ilGetInteger(IL_IMAGE_HEIGHT);
        desc.MipLevels = mipmap_level;
        desc.ArraySize = 1;
        desc.Format = DX11RenderInterface::FormatToDXGI_FORMAT(Format::R8G8B8A8UN_SRGB);
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        if (0 == mipmap_level)
        {
            desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
            desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
        }

        ID3D11Texture2D* texture = nullptr;
        if (FAILED(this->renderInterface
            ->getD3D11Device()->CreateTexture2D(&desc, nullptr, &texture)))
        {
            TOD_RETURN_TRACE(false);
        }

        this->renderInterface
            ->getD3D11DeviceContext()
            ->UpdateSubresource(
                texture, 0, nullptr, ilGetData()
                , desc.Width * ilGetInteger(IL_IMAGE_BPP), 0);

        D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
        memset(&srv_desc, 0, sizeof(srv_desc));
        srv_desc.Format = desc.Format;
        srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        if (0 == mipmap_level)
        {
            srv_desc.Texture2D.MipLevels = -1;
            srv_desc.Texture2D.MostDetailedMip = 0;
        }
        else
        {
            srv_desc.Texture2D.MipLevels = desc.MipLevels;
            srv_desc.Texture2D.MostDetailedMip = desc.MipLevels - 1;
        }
        this->renderInterface
            ->getD3D11Device()
            ->CreateShaderResourceView(
                texture, &srv_desc, &this->d3dtextureSRV);

        if (1 != mipmap_level)
        {
            this->renderInterface
                ->getD3D11DeviceContext()
                ->GenerateMips(this->d3dtextureSRV);
        }

        ilShutDown();

        this->d3dtexture2D = texture;
        this->width = desc.Width;
        this->height = desc.Height;
        this->format = Format::R8G8B8A8UN_SRGB;
    }

    return true;
}


//-----------------------------------------------------------------------------
void DX11Texture::use(int32 sampler_index)
{
}


//-----------------------------------------------------------------------------
void DX11Texture::setHorizontalWrap(TextureWrap value)
{

}


//-----------------------------------------------------------------------------
void DX11Texture::setVerticalWrap(TextureWrap value)
{

}


//-----------------------------------------------------------------------------
void DX11Texture::setMinFilter(TextureFilter value)
{

}


//-----------------------------------------------------------------------------
void DX11Texture::setMagFilter(TextureFilter value)
{

}


//-----------------------------------------------------------------------------
void DX11Texture::setBorderColor(const ColorF& color)
{

}


//-----------------------------------------------------------------------------
ID3D11ShaderResourceView* DX11Texture::getSRV()
{
    return this->d3dtextureSRV;
}

}