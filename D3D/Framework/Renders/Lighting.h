#pragma once

#define MAX_POINT_LIGHTS 256

struct PointLight
{
	Color Ambient;
	Color Diffuse;
	Color Specular;
	Color Emssive;

	Vector3 Position;
	float Range;

	float Intensity;
	float Padding[3];
};

class Lighting
{
public:
	static void Create();
	static void Delete();
	static Lighting* Get();

private:
	Lighting();
	~Lighting();

public:
	void Update();

public:
	void AddPointLight(PointLight& light);
	UINT PointLights(OUT PointLight* lights);
	UINT PointLightCount() { return pointLightCount; }
	PointLight& GetPointLight(UINT index);
	Transform* GetPointLightTransform(UINT index);

public:
	Color& Ambient() { return ambient; }
	Color& Specular() { return specular; }
	Vector3& Direction() { return direction; }
	Vector3& Position() { return position; }

private:
	static Lighting* instance;

private: //GlobalLight
	Color ambient = Color(0, 0, 0, 0);
	Color specular = Color(1, 1, 1, 1);
	Vector3 direction = Vector3(-1, -1, 1);
	Vector3 position = Vector3(0, 0, 0);

private: //PointLight
	UINT pointLightCount = 0;
	PointLight pointLights[MAX_POINT_LIGHTS];
	vector<Transform*> pointLightTransforms;
};