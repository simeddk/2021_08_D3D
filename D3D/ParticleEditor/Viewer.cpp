#include "stdafx.h"
#include "Viewer.h"


void Viewer::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(20, 0, 0);
	Context::Get()->GetCamera()->Position(1, 36, -85);
	
	particle = new Particle(L"Fire");

	shader = new Shader(L"28_AreaLighting.fxo");
	sky = new CubeSky(L"Environment/Mountain1024.dds");

	Mesh();
}

void Viewer::Destroy()
{
	SafeDelete(shader);
	SafeDelete(sky);

	SafeDelete(brick);
	SafeDelete(floor);
	SafeDelete(sphere);
	SafeDelete(grid);

	SafeDelete(particle);
}

void Viewer::Update()
{
	//램버트 테스트
	ImGui::SliderFloat3("LightDirection", Lighting::Get()->Direction(), -1, +1);
	
	grid->Update();
	sphere->Update();

	Vector3 P;
	sphere->GetTransform(0)->Position(&P);
	float moveSpeed = 20.0f;

	if (Mouse::Get()->Press(1) == false)
	{
		const Vector3& F = Context::Get()->GetCamera()->Forward();
		const Vector3& R = Context::Get()->GetCamera()->Right();
		const Vector3& U = Context::Get()->GetCamera()->Up();

		if (Keyboard::Get()->Press('W'))
			P += Vector3(F.x, 0.0f, F.z) * moveSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('S'))
			P -= Vector3(F.x, 0.0f, F.z) * moveSpeed * Time::Delta();

		if (Keyboard::Get()->Press('D'))
			P += R * moveSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('A'))
			P -= R * moveSpeed * Time::Delta();

		if (Keyboard::Get()->Press('E'))
			P += U * moveSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('Q'))
			P -= U * moveSpeed * Time::Delta();
	}
	sphere->GetTransform(0)->Position(P);
	sphere->UpdateSubResource();

	particle->Emit(P);
	particle->Update();
}

void Viewer::Render()
{
	sky->Render();

	brick->Render();
	sphere->Render();

	floor->Render();
	grid->Render();

	particle->Render();
}

void Viewer::Mesh()
{
	//Create Material
	{
		floor = new Material(shader);
		floor->DiffuseMap("Floor.png");
		floor->NormalMap("Floor_Normal.png");
		floor->NormalMap("Environment/Wave.dds");
		floor->SpecularMap("Floor_Specular.png");
		floor->Specular(1, 1, 1, 20);

		brick = new Material(shader);
		brick->DiffuseMap("Bricks.png");
		brick->NormalMap("Bricks_Normal.png");
		brick->SpecularMap("Bricks_Specular.png");
		brick->Specular(1, 1, 1, 20);
		brick->Emissive(0.15f, 0.15f, 0.15f, 0.3f);
	}

	//Create Mesh
	{
		Transform* transform = NULL;

		grid = new MeshRender(shader, new MeshPlane(15, 15));
		transform = grid->AddTransform();
		transform->Position(0, 0, 0);
		transform->Scale(20, 1, 20);

		sphere = new MeshRender(shader, new MeshSphere(0.5f, 20, 20));
		transform = sphere->AddTransform();
		transform->Position(0, 5, 0);
		transform->Scale(5, 5, 5);
	}

	sphere->UpdateSubResource();
	grid->UpdateSubResource();
}

void Viewer::Pass(UINT meshPass)
{
	for (MeshRender* mesh : meshes)
		mesh->Pass(meshPass);
}
