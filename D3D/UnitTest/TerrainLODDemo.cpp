#include "stdafx.h"
#include "TerrainLODDemo.h"

void TerrainLODDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(21, 0, 0);
	Context::Get()->GetCamera()->Position(126, 200, 100);
	((Freedom*)Context::Get()->GetCamera())->Speed(100);
	
	Perspective* perspective = Context::Get()->GetPerspective();
	perspective->Set(D3D::Width(), D3D::Height(), 0.1f, 10000.0f, Math::PI * 0.25f);

	shader = new Shader(L"48_TerrainLOD.fxo");
	terrain = new TerrainLOD(L"Terrain/Gray1920.dds");
	terrain->BaseMap(L"Terrain/Cliff (Layered Rock).jpg");
	
}

void TerrainLODDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(terrain);
}

void TerrainLODDemo::Update()
{
	ImGui::SliderFloat("Height Scale", &terrain->HeightScale(), 0.001f, 10.0f);
	ImGui::SliderFloat2("Distance", (float*)&terrain->Distance(), 1, 1000);

	static UINT pass = 1;
	ImGui::InputInt("Pass", (int*)&pass);
	pass = Math::Clamp<UINT>(pass, 0, 1);
	terrain->Pass(pass);

	terrain->Update();
}

void TerrainLODDemo::Render()
{
	terrain->Render();
}
