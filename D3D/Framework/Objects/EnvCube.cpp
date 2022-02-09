#include "Framework.h"
#include "EnvCube.h"

EnvCube::EnvCube(Shader * shader, float width, float height)
	: shader(shader)
	, width(width)
	, height(height)
{
	DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	//Create Texture2D - RTV
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = (UINT)width;
		desc.Height = (UINT)height;
		desc.ArraySize = 6;
		desc.Format = rtvFormat;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		Check(D3D::GetDevice()->CreateTexture2D(&desc, nullptr, &rtvTexture));
	}
	
	//Create RTV
	{
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		desc.Format = rtvFormat;
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.ArraySize = 6;

		Check(D3D::GetDevice()->CreateRenderTargetView(rtvTexture, &desc, &rtv));
	}

	//Create SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc.Format = rtvFormat;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		desc.TextureCube.MipLevels = 1;

		Check(D3D::GetDevice()->CreateShaderResourceView(rtvTexture, &desc, &srv));
	}

	DXGI_FORMAT dsvFormat = DXGI_FORMAT_D32_FLOAT;
	//Create Texture2D - DSV
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = (UINT)width;
		desc.Height = (UINT)height;
		desc.ArraySize = 6;
		desc.Format = dsvFormat;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		Check(D3D::GetDevice()->CreateTexture2D(&desc, nullptr, &dsvTexture));
	}

	//Create DSV
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		desc.Format = dsvFormat;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.ArraySize = 6;

		Check(D3D::GetDevice()->CreateDepthStencilView(dsvTexture, &desc, &dsv));
	}

	viewport = new Viewport((float)this->width, (float)this->height);
	projection = new Perspective(1, 1);

	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_EnvCube");

	sSrv = shader->AsSRV("EnvCubeMap");
}

EnvCube::~EnvCube()
{
	SafeRelease(rtvTexture);
	SafeRelease(rtv);
	SafeRelease(srv);

	SafeRelease(dsvTexture);
	SafeRelease(dsv);

	SafeDelete(viewport);
	SafeDelete(projection);
}

void EnvCube::PreRender(Vector3 & position, Vector3 & scale, float zNear, float zFar, float fov)
{
	//Create ViewMatrix -> Cbuffer Subresource
	{
		//Eye
		float x = position.x;
		float y = position.y;
		float z = position.z;

		//At
		struct LookAt
		{
			Vector3 At;
			Vector3 Up;
		} lookAt[6];

		lookAt[0] = { Vector3(x + scale.x, y, z), Vector3(0, 1, 0)};//Right
		lookAt[1] = { Vector3(x - scale.x, y, z), Vector3(0, 1, 0) };//-Right
		lookAt[2] = { Vector3(x, y + scale.y, z), Vector3(0, 0, -1) };//Up
		lookAt[3] = { Vector3(x, y - scale.y, z), Vector3(0, 0, 1) };//-Up
		lookAt[4] = { Vector3(x, y, z + scale.z), Vector3(0, 1, 0) };//Forward
		lookAt[5] = { Vector3(x, y, z - scale.z), Vector3(0, 1, 0) };;//-Forward

		for (UINT i = 0; i < 6; i++)
			D3DXMatrixLookAtLH(&desc.Views[i], &position, &lookAt[i].At, &lookAt[i].Up);
	}

	//Create ProjectionMatrix -> Cbuffer Subresource
	((Perspective*)projection)->Set(1, 1, zNear, zFar, fov * Math::PI);
	projection->GetMatrix(&desc.Projection);

	//Cbuffer Render
	buffer->Render();
	sBuffer->SetConstantBuffer(buffer->Buffer());

	//OMSet
	D3D::Get()->SetRenderTarget(rtv, dsv);
	D3D::Get()->Clear(Color(0, 0, 0, 1), rtv, dsv);
	viewport->RSSetViewport();
}

void EnvCube::Render()
{
	sSrv->SetResource(srv);
}
