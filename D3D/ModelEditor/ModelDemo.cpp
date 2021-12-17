#include "stdafx.h"
#include "ModelDemo.h"
#include "Converter.h"

void ModelDemo::Initialize()
{
	shader = new Shader(L"15_Model.fxo");

	Tank();

	sky = new CubeSky(L"Environment/SnowCube1024.dds");
	sky->Pass(2);

	planeShader = new Shader(L"12_Mesh.fxo");
	plane = new MeshPlane(planeShader, 6, 6);
	plane->GetTransform()->Scale(12, 1, 12);
	plane->DiffuseMap(L"Floor.png");
}

void ModelDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(tank);
	
	SafeDelete(sky);
	SafeDelete(planeShader);
	SafeDelete(plane);
}

void ModelDemo::Update()
{
	sky->Update();
	plane->Update();

	if (tank != nullptr)
		tank->Update();
}

void ModelDemo::Render()
{
	sky->Render();
	plane->Render();

	if (tank != nullptr)
		tank->Render();
}

void ModelDemo::Tank()
{
	tank = new ModelRender(shader);
	tank->ReadMesh(L"Tank/Tank");
}
