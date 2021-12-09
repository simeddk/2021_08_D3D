#include "stdafx.h"
#include "TerrainDemo.h"

void TerrainDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(6, 0, 0);
	Context::Get()->GetCamera()->Position(110, 50, -110);
	((Freedom*)Context::Get()->GetCamera())->Speed(50, 2);

	shader = new Shader(L"11_Terrain.fx");

	terrain = new Terrain(shader, L"Terrain/Gray256.png");
	terrain->BaseMap(L"Terrain/Cliff (Sandstone).jpg");
}

void TerrainDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(terrain);
}

void TerrainDemo::Update()
{
	//조명 방향 테스트
	static Vector3 LightDirection = Vector3(-1, -1, +1);
	ImGui::SliderFloat3("Light Direction", (float*)LightDirection, -1, 1);
	shader->AsVector("LightDirection")->SetFloatVector(LightDirection);

	//파이프라인 변경 테스트
	static UINT pass = shader->PassCount() - 1;
	ImGui::InputInt("Pass", (int*)&pass);
	pass %= shader->PassCount();
	terrain->Pass() = pass;

	//알베도 or 램버트 모드 테스트
	if (pass == 5)
	{
		static UINT albedo = 1;
		ImGui::RadioButton("Albedo", (int*)&albedo, 1);
		ImGui::RadioButton("Lambert", (int*)&albedo, 2);
		shader->AsScalar("Albedo")->SetInt(albedo);
	}
	

	terrain->Update();
}

void TerrainDemo::Render()
{
	terrain->Render();
}


