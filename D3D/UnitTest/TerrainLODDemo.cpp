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
	terrain->NormalMap(L"Terrain/Cliff (Layered Rock)_NormalMap.png");
	
}

void TerrainLODDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(terrain);
}

void TerrainLODDemo::Update()
{

	ImGui::SliderFloat3("LightDirection", Lighting::Get()->Direction(), -1, 1);
	ImGui::SliderFloat("Height Scale", &terrain->HeightScale(), 0.001f, 10.0f);
	ImGui::SliderFloat2("Distance", (float*)&terrain->Distance(), 1, 1000);

	/*static float zFar = 500;
	static float fov = 0.25f;
	ImGui::InputFloat("Far", &zFar, 1.0f);
	ImGui::InputFloat("Fov", &fov, 1e-3f);
	terrain->GetPerspective()->Set(1024, 768, 1, zFar, Math::PI * fov);

	static Vector3 position;
	ImGui::SliderFloat3("Camera Position", (float*)&position, -100.0f, 100.0);
	terrain->GetCamera()->Position(position);

	static Vector3 rotation;
	ImGui::SliderFloat3("Camera Rotation", (float*)&rotation, -Math::PI, Math::PI);
	terrain->GetCamera()->Rotation(rotation);*/

	static UINT pass = 0;
	ImGui::InputInt("Pass", (int*)&pass);
	pass = Math::Clamp<UINT>(pass, 0, 1);
	terrain->Pass(pass);

	terrain->Update();
}

void TerrainLODDemo::Render()
{
	terrain->Render();
}
