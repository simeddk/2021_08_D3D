#include "Framework.h"
#include "Shadow.h"

Shadow::Shadow(Shader * shader, Vector3 & at, float radius, float width, float height)
	: shader(shader)
	, at(at)
	, radius(radius)
	, width(width)
	, height(height)
{
	renderTarget = new RenderTarget(width, height);
	depthStencil = new DepthStencil(width, height);
	viewport = new  Viewport(width, height);
	
	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Shadow");

	sShadowMap = shader->AsSRV("ShadowMap");
}

Shadow::~Shadow()
{
	SafeDelete(renderTarget);
	SafeDelete(depthStencil);
	SafeDelete(viewport);
	SafeDelete(buffer);
}

void Shadow::PreRender()
{
	renderTarget->PreRender(depthStencil);
	viewport->RSSetViewport();

	Vector3 up = Vector3(0, 1, 0);
	Vector3 direction = Lighting::Get()->Direction();
	Vector3 eye = direction * radius * -2.0f;

	D3DXMatrixLookAtLH(&desc.View, &eye, &at, &up);

	Vector3 center;
	D3DXVec3TransformCoord(&center, &at, &desc.View);

	float l = center.x - radius;
	float r = center.x + radius;
	float t = center.y + radius;
	float b = center.y - radius;
	float f = center.z + radius;
	float n = center.z - radius;

	D3DXMatrixOrthoLH(&desc.Projection, r - l, t - b, n, f);

	buffer->Render();
	sBuffer->SetConstantBuffer(buffer->Buffer());

	sShadowMap->SetResource(depthStencil->SRV());
}
