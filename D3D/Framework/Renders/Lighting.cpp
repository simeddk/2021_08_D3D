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
