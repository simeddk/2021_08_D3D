#include "stdafx.h"
#include "ModelDemo.h"
#include "Converter.h"

void ModelDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(7, 2, 0);
	Context::Get()->GetCamera()->Position(1, 5, -14);
	shader = new Shader(L"15_Model.fxo");

	Tank();
	Kachujin();
	Tower();
	Airplane();

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
	SafeDelete(kachujin);
	SafeDelete(tower);
	SafeDelete(airplane);
	
	SafeDelete(sky);
	SafeDelete(planeShader);
	SafeDelete(plane);
}

void ModelDemo::Update()
{
	//램버트 테스트
	static Vector3 LightDirection = Vector3(-1, -1, +1);
	ImGui::SliderFloat3("LightDirection", LightDirection, -1, +1);
	shader->AsVector("LightDirection")->SetFloatVector(LightDirection);
	planeShader->AsVector("LightDirection")->SetFloatVector(LightDirection);

	//와이어프레임 테스트
	static UINT pass = 0;
	ImGui::InputInt("Pass", (int *)&pass);
	pass %= 2;
	tank->Pass(pass);
	kachujin->Pass(pass);
	tower->Pass(pass);
	airplane->Pass(pass);

	//이동 테스트
	{
		Vector3 P;
		tank->GetTransform()->Position(&P);
	
		if (Keyboard::Get()->Press(VK_UP))
			P += Context::Get()->GetCamera()->Forward() * 20.0f * Time::Delta();
		else if (Keyboard::Get()->Press(VK_DOWN))
			P -= Context::Get()->GetCamera()->Forward() * 20.0f * Time::Delta();
		if (Keyboard::Get()->Press(VK_RIGHT))
			P += Context::Get()->GetCamera()->Right() * 20.0f * Time::Delta();
		else if (Keyboard::Get()->Press(VK_LEFT))
			P -= Context::Get()->GetCamera()->Right() * 20.0f * Time::Delta();
	
		P.y = 0.0f;
		tank->GetTransform()->Position(P);
	}

	sky->Update();
	plane->Update();

	if (tank != nullptr)
		tank->Update();

	if (kachujin != nullptr)
		kachujin->Update();

	if (tower != nullptr)
		tower->Update();

	if (airplane != nullptr)
		airplane->Update();
}

void ModelDemo::Render()
{
	sky->Render();
	plane->Render();

	if (tank != nullptr)
		tank->Render();

	if (kachujin != nullptr)
		kachujin->Render();

	if (tower != nullptr)
		tower->Render();

	if (airplane != nullptr)
		airplane->Render();
}

void ModelDemo::Tank()
{
	tank = new ModelRender(shader);
	tank->ReadMesh(L"Tank/Tank");
}

void ModelDemo::Kachujin()
{
	kachujin = new ModelRender(shader);
	kachujin->ReadMesh(L"Kachujin/Mesh");
	kachujin->GetTransform()->Scale(0.01f, 0.01f, 0.01f);
	kachujin->GetTransform()->Position(5, 0, 0);
}

void ModelDemo::Tower()
{
	tower = new ModelRender(shader);
	tower->ReadMesh(L"Tower/Tower");
	tower->GetTransform()->Scale(0.01f, 0.01f, 0.01f);
	tower->GetTransform()->Position(-5, 0, 0);
}

void ModelDemo::Airplane()
{
	airplane = new ModelRender(shader);
	airplane->ReadMesh(L"B787/Airplane");
	airplane->GetTransform()->Scale(0.001f, 0.001f, 0.001f);
	airplane->GetTransform()->Position(-10, 0, 0);
}
