#include "stdafx.h"
#include "CubeMapDemo.h"

//TODO
//Å¥ºê¸Ê uv¸ÊÇÎ ¿ø¸®
void CubeMapDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(20, 0, 0);
	Context::Get()->GetCamera()->Position(1, 36, -85);

	shader = new Shader(L"12_Mesh.fx");
	
	CreateMesh();

	sDirection = shader->AsVector("LightDirection");

	cubeMapShader = new Shader(L"13_CubeMap.fx");
	cubeMap = new CubeMap(cubeMapShader);
	cubeMap->Texture(L"Environment/SnowCube1024.dds");
	cubeMap->Position(0, 20, 0);
	cubeMap->Scale(10, 10, 10);
}

void CubeMapDemo::Destroy()
{
	SafeDelete(shader);

	SafeDelete(quad);
	SafeDelete(plane);
	SafeDelete(cube);

	for (UINT i = 0; i < 10; i++)
	{
		SafeDelete(cylinders[i]);
		SafeDelete(spheres[i]);
	}

	SafeDelete(cubeMapShader);
	SafeDelete(cubeMap);
}

void CubeMapDemo::Update()
{
	ImGui::SliderFloat3("Direction", direction, -1, +1);
	sDirection->SetFloatVector(direction);

	quad->Update();
	plane->Update();

	cube->Update();

	for (UINT i = 0; i < 10; i++)
	{
		cylinders[i]->Update();
		spheres[i]->Update();
	}

	cubeMap->Update();
}

void CubeMapDemo::Render()
{
	static bool bWireframe = false;
	ImGui::Checkbox("Wire Frame", &bWireframe);

	quad->Pass(bWireframe ? 1 : 0);
	plane->Pass(bWireframe ? 1 : 0);
	cube->Pass(bWireframe ? 1 : 0);

	quad->Render();
	plane->Render();

	cube->Render();

	for (UINT i = 0; i < 10; i++)
	{
		cylinders[i]->Pass(bWireframe ? 1 : 0);
		cylinders[i]->Render();

		spheres[i]->Pass(bWireframe ? 1 : 0);
		spheres[i]->Render();
	}

	cubeMap->Render();
}

void CubeMapDemo::CreateMesh()
{
	quad = new MeshQuad(shader);
	quad->DiffuseMap(L"Box.png");

	plane = new MeshPlane(shader, 2.5f, 2.5f);
	plane->Scale(12, 1, 12);
	plane->DiffuseMap(L"Floor.png");

	cube = new MeshCube(shader);
	cube->Position(0, 5, 0);
	cube->Scale(20, 10, 20);
	cube->DiffuseMap(L"Stones.png");

	for (UINT i = 0; i < 5; i++)
	{
		//Cylinders
		cylinders[i * 2 + 0] = new MeshCylinder(shader, 0.3f, 0.5f, 3.0f, 20, 20);
		cylinders[i * 2 + 0]->Position(-30, 6, (float)i * 15.0f - 15.0f);
		cylinders[i * 2 + 0]->Scale(5, 5, 5);
		cylinders[i * 2 + 0]->DiffuseMap(L"Bricks.png");

		cylinders[i * 2 + 1] = new MeshCylinder(shader, 0.3f, 0.5f, 3.0f, 20, 20);
		cylinders[i * 2 + 1]->Position(+30, 6, (float)i * 15.0f - 15.0f);
		cylinders[i * 2 + 1]->Scale(5, 5, 5);
		cylinders[i * 2 + 1]->DiffuseMap(L"Bricks.png");

		//Spheres
		spheres[i * 2 + 0] = new MeshSphere(shader, 0.5f);
		spheres[i * 2 + 0]->Position(-30, 15.5f, (float)i * 15.0f - 15.0f);
		spheres[i * 2 + 0]->Scale(5, 5, 5);
		spheres[i * 2 + 0]->DiffuseMap(L"Wall.png");

		spheres[i * 2 + 1] = new MeshSphere(shader, 0.5f);
		spheres[i * 2 + 1]->Position(+30, 15.5f, (float)i * 15.0f - 15.0f);
		spheres[i * 2 + 1]->Scale(5, 5, 5);
		spheres[i * 2 + 1]->DiffuseMap(L"Wall.png");
	}
}


