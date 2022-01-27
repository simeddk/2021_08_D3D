#include "Framework.h"
#include "DepthStencil.h"

DepthStencil::DepthStencil(float width, float height, bool bUseStencil)
{
	this->width = (width < 1) ? D3D::Width() : width;
	this->height = (height < 1) ? D3D::Height() : height;

	
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.Width = (UINT)this->width;
	textureDesc.Height = (UINT)this->height;
	textureDesc.ArraySize = 1;
	textureDesc.Format = bUseStencil ? DXGI_FORMAT_R24G8_TYPELESS : DXGI_FORMAT_R32_TYPELESS;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.MipLevels = 1;
	textureDesc.SampleDesc.Count = 1;
	Check(D3D::GetDevice()->CreateTexture2D(&textureDesc, nullptr, &texture));
	
}

DepthStencil::~DepthStencil()
{
}
