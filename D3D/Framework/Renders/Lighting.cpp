#include "Framework.h"
#include "Lighting.h"

Lighting* Lighting::instance = nullptr;

void Lighting::Create()
{
	assert(instance == nullptr);
	instance = new Lighting();
}

void Lighting::Delete()
{
	SafeDelete(instance);
}

Lighting * Lighting::Get()
{
	assert(instance != nullptr);
	return instance;
}

Lighting::Lighting()
{
}

Lighting::~Lighting()
{
}

void Lighting::Update()
{
	for (UINT i = 0; i < pointLightCount; i++)
		pointLightTransforms[i]->Position(&pointLights[i].Position);

	for (UINT i = 0; i < spotLightCount; i++)
		spotLightTransforms[i]->Position(&spotLights[i].Position);
}

void Lighting::AddPointLight(PointLight & light)
{
	pointLights[pointLightCount] = light;
	pointLightCount++;

	Transform* t = new Transform();
	t->Position(pointLights[pointLightCount - 1].Position);
	pointLightTransforms.push_back(t);
}

UINT Lighting::PointLights(OUT PointLight * lights)
{
	memcpy(lights, pointLights, sizeof(PointLight) * pointLightCount);

	return pointLightCount;
}

PointLight & Lighting::GetPointLight(UINT index)
{
	return pointLights[index];
}

Transform * Lighting::GetPointLightTransform(UINT index)
{
	return pointLightTransforms[index];
}

void Lighting::AddSpotLight(SpotLight & light)
{
	spotLights[spotLightCount] = light;
	spotLightCount++;

	Transform* t = new Transform();
	t->Position(spotLights[spotLightCount - 1].Position);

	spotLightTransforms.push_back(t);
}

UINT Lighting::SpotLights(OUT SpotLight * lights)
{
	memcpy(lights, spotLights, sizeof(SpotLight) * spotLightCount);

	return spotLightCount;
}

SpotLight & Lighting::GetSpotLight(UINT index)
{
	return spotLights[index];
}

Transform * Lighting::GetSpotLightTransform(UINT index)
{
	return spotLightTransforms[index];
}
