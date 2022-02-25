#include "Framework.h"
#include "PerFrame.h"

PerFrame::PerFrame(Shader * shader)
	:shader(shader)
{
	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_PerFrame");

	lightBuffer = new ConstantBuffer(&lightDesc, sizeof(LightDesc));
	sLightBuffer = shader->AsConstantBuffer("CB_Light");

	pointLightBuffer = new ConstantBuffer(&pointLightsDesc, sizeof(PointLightsDesc));
	sPointLightBuffer = shader->AsConstantBuffer("CB_PointLights");

	spotLightBuffer = new ConstantBuffer(&spotLightsDesc, sizeof(SpotLightsDesc));
	sSpotLightBuffer = shader->AsConstantBuffer("CB_SpotLights");

	fogBuffer = new ConstantBuffer(&fogDesc, sizeof(FogDesc));
	sFogBuffer = shader->AsConstantBuffer("CB_Fog");
}

PerFrame::~PerFrame()
{
	SafeDelete(buffer);
	SafeDelete(lightBuffer);
	SafeDelete(pointLightBuffer);
	SafeDelete(spotLightBuffer);
	SafeDelete(fogBuffer);
}

void PerFrame::Update()
{
	desc.Time = Time::Get()->Running();

	desc.View = Context::Get()->View();
	D3DXMatrixInverse(&desc.ViewInverse, nullptr, &desc.View);

	desc.Projection = Context::Get()->Projection();
	desc.VP = desc.View * desc.Projection;

	lightDesc.Ambient = Lighting::Get()->Ambient();
	lightDesc.Specular = Lighting::Get()->Specular();
	lightDesc.Direction = Lighting::Get()->Direction();
	lightDesc.Position = Lighting::Get()->Position();

	pointLightsDesc.Count = Lighting::Get()->PointLights(pointLightsDesc.Lights);
	spotLightsDesc.Count = Lighting::Get()->SpotLights(spotLightsDesc.Lights);

	fogDesc.FogColor = Lighting::Get()->FogColor();
	fogDesc.FogDistance = Lighting::Get()->FogDistance();
	fogDesc.FogDensity = Lighting::Get()->FogDensity();
	fogDesc.FogType = Lighting::Get()->FogType();
}

void PerFrame::Render()
{
	memcpy(desc.Culling, Context::Get()->Culling(), sizeof(Plane) * 4);
	desc.Clipping = Context::Get()->Clipping();

	buffer->Render();
	sBuffer->SetConstantBuffer(buffer->Buffer());

	lightBuffer->Render();
	sLightBuffer->SetConstantBuffer(lightBuffer->Buffer());

	pointLightBuffer->Render();
	sPointLightBuffer->SetConstantBuffer(pointLightBuffer->Buffer());

	spotLightBuffer->Render();
	sSpotLightBuffer->SetConstantBuffer(spotLightBuffer->Buffer());

	fogBuffer->Render();
	sFogBuffer->SetConstantBuffer(fogBuffer->Buffer());
}
