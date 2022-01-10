#include "stdafx.h"
#include "ModelDemo.h"
#include "Converter.h"

void ModelDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(7, 2, 0);
	Context::Get()->GetCamera()->Position(1, 5, -14);
	shader = new Shader(L"24_integration.fxo");

	Tank();
	Tower();
	Airplane();

}

void ModelDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(tank);
	SafeDelete(tower);
	SafeDelete(airplane);
}

void ModelDemo::Update()
{
	//램버트 테스트
	{
		static Vector3 LightDirection = Vector3(-1, -1, +1);
		ImGui::SliderFloat3("LightDirection", LightDirection, -1, +1);
		shader->AsVector("LightDirection")->SetFloatVector(LightDirection);
	}
	
	//특정 인스턴싱 편집 테스트
	{
		static int index = 0;
		ImGui::SliderInt("InstanceID", &index, 0, tower->TransformCount() - 1);
		Transform* transfrom = tower->GetTransform(index);
		Vector3 R;
		transfrom->RotationDegree(&R);
		ImGui::SliderFloat3("Rotation", R, -180, 180);
		transfrom->RotationDegree(R);
		tower->UpdateSubResource();

		airplane->SetColor(20, Color(1, 0, 0, 1));
		airplane->SetColor(21, Color(0, 1, 0, 1));
		airplane->UpdateSubResource();
	}
	
	if (tank != nullptr)
		tank->Update();
	
	if (tower != nullptr)
		tower->Update();

	if (airplane != nullptr)
		airplane->Update();
}

void ModelDemo::Render()
{
	if (tank != nullptr)
	{
		tank->Pass(1);
		tank->Render();
	}
	
	if (tower != nullptr)
	{
		tower->Pass(1);
		tower->Render();
	}

	if (airplane != nullptr)
	{
		airplane->Pass(1);
		airplane->Render();
	}
}

void ModelDemo::Tank()
{
	tank = new ModelRender(shader);
	tank->ReadMesh(L"Tank/Tank");
	tank->ReadMaterial(L"Tank/Tank");

	for (float x = -50; x <= 50; x += 2.5f)
	{
		Transform* transform = tank->AddTransform();
		transform->Position(Vector3(x, 0, 5.0f));
		transform->RotationDegree(0, Math::Random(-180, 180), 0);
		transform->Scale(Vector3(0.1f, 0.1f, 0.1f));
	}
	tank->UpdateSubResource();
}

void ModelDemo::Tower()
{
	tower = new ModelRender(shader);
	tower->ReadMesh(L"Tower/Tower");
	tower->ReadMaterial(L"Tower/Tower");
	
	for (float x = -50; x <= 50; x += 2.5f)
	{
		Transform* transform = tower->AddTransform();
		transform->Position(Vector3(x, 0, 7.5f));
		transform->RotationDegree(0, Math::Random(-180, 180), 0);
		transform->Scale(Vector3(0.003f, 0.003f, 0.003f));
	}
	tower->UpdateSubResource();
}

void ModelDemo::Airplane()
{
	airplane = new ModelRender(shader);
	airplane->ReadMesh(L"B787/Airplane");
	airplane->ReadMaterial(L"B787/Airplane");
	
	for (float x = -50; x <= 50; x += 2.5f)
	{
		Transform* transform = airplane->AddTransform();
		transform->Position(Vector3(x, 0, 2.5f));
		transform->RotationDegree(0, Math::Random(-180, 180), 0);
		transform->Scale(Vector3(0.00025f, 0.00025f, 0.00025f));
	}
	airplane->UpdateSubResource();
}
