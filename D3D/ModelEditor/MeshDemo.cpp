#include "stdafx.h"
#include "MeshDemo.h"
#include "Converter.h"

void MeshDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(13, 70, 0);
	Context::Get()->GetCamera()->Position(-20, 1, -3);
	
	shader = new Shader(L"24_integration.fxo");

	planeMaterial = new Material(shader);
	planeMaterial->DiffuseMap(L"Floor.png");

	plane = new MeshRender(shader, new MeshPlane(10, 2));
	plane->AddTransform()->Scale(10, 1, 2);
	plane->UpdateSubResource();

	boxMaterial = new Material(shader);
	boxMaterial->DiffuseMap(L"Box.png");

	box = new MeshRender(shader, new MeshCube());
	for (float x = -50; x <= 50; x += 2.5f)
	{
		Transform* transform = box->AddTransform();
		transform->Scale(0.25f, 0.25f, 0.25f);
		transform->Position(Vector3(x, 0.125f, 0.0f));
		transform->Rotation(0, Math::Random(-(float)D3DX_PI, (float)D3DX_PI), 0);
	}
	box->UpdateSubResource();
}

void MeshDemo::Destroy()
{
	SafeDelete(shader);

	SafeDelete(planeMaterial);
	SafeDelete(plane);

	SafeDelete(boxMaterial);
	SafeDelete(box);
}

void MeshDemo::Update()
{
	//램버트 테스트
	static Vector3 LightDirection = Vector3(-1, -1, +1);
	ImGui::SliderFloat3("LightDirection", LightDirection, -1, +1);
	shader->AsVector("LightDirection")->SetFloatVector(LightDirection);
	
	plane->Update();
	box->Update();
}

void MeshDemo::Render()
{
	planeMaterial->Render();
	plane->Render();

	boxMaterial->Render();
	box->Render();
}
