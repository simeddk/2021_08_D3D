#include "stdafx.h"
#include "MeshDemo.h"

void MeshDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(25, 2, 0);
	Context::Get()->GetCamera()->Position(65, 14, 42);

	shader = new Shader(L"12_Mesh.fx");
	
	CreateMesh();

	sDirection = shader->AsVector("LightDirection");
}

void MeshDemo::Destroy()
{
	SafeDelete(shader);

	SafeDelete(quad);
	SafeDelete(plane);
}

void MeshDemo::Update()
{
	ImGui::SliderFloat3("Direction", direction, -1, +1);
	sDirection->SetFloatVector(direction);

	quad->Update();
	plane->Update();

}

void MeshDemo::Render()
{
	static bool bWireframe = false;
	ImGui::Checkbox("Wire Frame", &bWireframe);

	quad->Pass(bWireframe ? 1 : 0);

	quad->Render();
	plane->Render();
}

void MeshDemo::CreateMesh()
{
	quad = new MeshQuad(shader);
	quad->DiffuseMap(L"Box.png");

	plane = new MeshPlane(shader);
	plane->Scale(12, 1, 12);
	plane->DiffuseMap(L"Floor.png");
}


