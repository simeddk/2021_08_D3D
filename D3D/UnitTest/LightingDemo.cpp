#include "stdafx.h"
#include "LightingDemo.h"

void LightingDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(20, 0, 0);
	Context::Get()->GetCamera()->Position(1, 36, -85);
	((Freedom*)Context::Get()->GetCamera())->Speed(50, 2);
	
	shader = new Shader(L"26_Lighting.fxo");
	sky = new CubeSky(L"Environment/SunsetCube1024.dds");

	Mesh();
	Airplane();
	Kachujin();
	Weapon();
}

void LightingDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(sky);

	//Materials
	SafeDelete(floor);
	SafeDelete(stone);
	SafeDelete(brick);
	SafeDelete(wall);

	//Mesh
	SafeDelete(plane);
	SafeDelete(cube);
	SafeDelete(cylinder);
	SafeDelete(sphere);

	//Mesh
	SafeDelete(airplane);
	SafeDelete(kachujin);
	SafeDelete(weapon);
}

void LightingDemo::Update()
{
	//����Ʈ �׽�Ʈ
	ImGui::SliderFloat3("LightDirection", Context::Get()->Direction(), -1, +1);

	sky->Update();

	plane->Update();
	cube->Update();
	cylinder->Update();
	sphere->Update();

	airplane->Update();
	kachujin->Update();

	Matrix worlds[MAX_MODEL_TRANSFORMS];
	for (UINT i = 0; i < kachujin->GetTransformCount(); i++)
	{
		kachujin->GetAttachBones(i, worlds);
		weapon->GetTransform(i)->World(weaponInitTransform->World() * worlds[40]);
	}
	weapon->UpdateSubResource();
	weapon->Update();

}

void LightingDemo::Render()
{
	sky->Render();

	Pass(0);

	wall->Render();
	sphere->Render();

	brick->Render();
	cylinder->Render();

	stone->Render();
	cube->Render();

	floor->Render();
	plane->Render();

	airplane->Render();
	kachujin->Render();
	weapon->Render();
}

void LightingDemo::Mesh()
{
	//Create Material
	{
		//TODO �ݻ纤�Ͱ� ��Į����..
		floor = new Material(shader);
		floor->DiffuseMap("Floor.png");
		floor->Specular(1, 1, 1, 20);

		stone = new Material(shader);
		stone->DiffuseMap("Stones.png");
		stone->Specular(1, 1, 1, 20);

		brick = new Material(shader);
		brick->DiffuseMap("Bricks.png");
		brick->Specular(1, 1, 1, 20);

		wall = new Material(shader);
		wall->DiffuseMap("Wall.png");
		wall->Specular(1, 1, 1, 20);
	}

	//Create Mesh
	{
		Transform* transform = nullptr;

		cube = new MeshRender(shader, new MeshCube());
		transform = cube->AddTransform();
		transform->Position(0, 5, 0);
		transform->Scale(20, 10, 20);

		plane = new MeshRender(shader, new MeshPlane(2.5f, 2.5f));
		transform = plane->AddTransform();
		transform->Scale(12, 1, 12);

		cylinder = new MeshRender(shader, new MeshCylinder(0.3f, 0.5f, 3.0f, 20, 20));
		sphere = new MeshRender(shader, new MeshSphere(0.5f, 20, 20));
		for (UINT i = 0; i < 5; i++)
		{
			transform = cylinder->AddTransform();
			transform->Position(-30, 6, (float)i * 15.0f - 15.0f);
			transform->Scale(5, 5, 5);

			transform = cylinder->AddTransform();
			transform->Position(+30, 6, (float)i * 15.0f - 15.0f);
			transform->Scale(5, 5, 5);

			transform = sphere->AddTransform();
			transform->Position(-30, 15.5f, (float)i * 15.0f - 15.0f);
			transform->Scale(5, 5, 5);

			transform = sphere->AddTransform();
			transform->Position(+30, 15.5f, (float)i * 15.0f - 15.0f);
			transform->Scale(5, 5, 5);
		}
	}

	cube->UpdateSubResource();
	plane->UpdateSubResource();
	cylinder->UpdateSubResource();
	sphere->UpdateSubResource();

	meshes.push_back(cube);
	meshes.push_back(plane);
	meshes.push_back(cylinder);
	meshes.push_back(sphere);
}

void LightingDemo::Airplane()
{
	airplane = new ModelRender(shader);
	airplane->ReadMesh(L"B787/Airplane");
	airplane->ReadMaterial(L"B787/Airplane");

	Transform* transform = airplane->AddTransform();
	transform->Position(Vector3(2.0f, 9.91f, 2.0f));
	transform->Scale(Vector3(0.004f, 0.004f, 0.004f));
	airplane->UpdateSubResource();

	models.push_back(airplane);
}

void LightingDemo::Kachujin()
{
	kachujin = new ModelAnimator(shader);
	kachujin->ReadMesh(L"Kachujin/Mesh");
	kachujin->ReadMaterial(L"Kachujin/Mesh");
	kachujin->ReadClip(L"Kachujin/Idle");
	kachujin->ReadClip(L"Kachujin/Walk");
	kachujin->ReadClip(L"Kachujin/Run");
	kachujin->ReadClip(L"Kachujin/Slash");
	kachujin->ReadClip(L"Kachujin/Uprock");

	Transform* transform = nullptr;

	transform = kachujin->AddTransform();
	transform->Position(0, 0, -30);
	transform->Scale(0.075f, 0.075f, 0.075f);
	kachujin->PlayTweenMode(0, 0, 1.0f);

	transform = kachujin->AddTransform();
	transform->Position(-15, 0, -30);
	transform->Scale(0.075f, 0.075f, 0.075f);
	kachujin->PlayTweenMode(1, 1, 1.0f);

	transform = kachujin->AddTransform();
	transform->Position(-30, 0, -30);
	transform->Scale(0.075f, 0.075f, 0.075f);
	kachujin->PlayTweenMode(2, 2, 0.75f);

	transform = kachujin->AddTransform();
	transform->Position(15, 0, -30);
	transform->Scale(0.075f, 0.075f, 0.075f);
	kachujin->PlayBlendMode(3, 0, 1, 2);
	kachujin->SetBlendAlpha(3, 1.75f);

	transform = kachujin->AddTransform();
	transform->Position(30, 0, -32.5f);
	transform->Scale(0.075f, 0.075f, 0.075f);
	kachujin->PlayTweenMode(4, 4, 0.75f);

	kachujin->UpdateSubResource();

	animators.push_back(kachujin);

}

void LightingDemo::Weapon()
{
	weapon = new ModelRender(shader);
	weapon->ReadMesh(L"Weapon/Sword");
	weapon->ReadMaterial(L"Weapon/Sword");

	UINT count = kachujin->GetTransformCount();
	for (UINT i = 0; i < count; i++)
		weapon->AddTransform();

	weapon->UpdateSubResource();
	models.push_back(weapon);


	weaponInitTransform = new Transform();
	weaponInitTransform->Position(-2.9f, 1.45f, -6.45f);
	weaponInitTransform->Scale(0.5f, 0.5f, 0.75f);
	weaponInitTransform->Rotation(0, 0, 1);
}

void LightingDemo::Pass(UINT val)
{
	for (MeshRender* mesh : meshes)
		mesh->Pass(val);

	val++;
	for (ModelRender* model : models)
		model->Pass(val);

	val++;
	for (ModelAnimator* animation : animators)
		animation->Pass(val);
}
