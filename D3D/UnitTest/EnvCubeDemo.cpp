#include "stdafx.h"
#include "EnvCubeDemo.h"

void EnvCubeDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(20, 0, 0);
	Context::Get()->GetCamera()->Position(1, 36, -85);
	((Freedom*)Context::Get()->GetCamera())->Speed(50, 2);
	
	shader = new Shader(L"38_EnvCube.fxo");
	envCube = new EnvCube(shader, 256.0f, 256.0f);

	sky = new CubeSky(L"Environment/SunsetCube1024.dds", shader);


	Mesh();
	Airplane();
	Kachujin();
	Kachujin2();
	Weapon();

	PointLights();
	SpotLights();
}

void EnvCubeDemo::Destroy()
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
	SafeDelete(kachujin2);
	SafeDelete(weapon);

	SafeDelete(envCube);
	SafeDelete(sphere2);
}

void EnvCubeDemo::Update()
{
	//램버트 테스트
	ImGui::SliderFloat3("LightDirection", Lighting::Get()->Direction(), -1, +1);

	//굴절, 프리넬 테스트
	{
		ImGui::InputInt("Type", (int*)&envCube->Type());
		envCube->Type() %= 4;

		ImGui::SliderFloat("Alpha", &envCube->Alpha(), 0, 1);
		ImGui::InputFloat("RefractAmount", &envCube->RefractAmount(), 0.01f);

		ImGui::InputFloat("FresnelAmount", &envCube->FresnelAmount(), 0.01f);
		ImGui::InputFloat("FresnelBias", &envCube->FresnelBias(), 0.01f);
		ImGui::InputFloat("FresnelScale", &envCube->FresnelScale(), 0.01f);
	}

	//Sphere2 이동
	{
		Vector3 position;
		//sphere2->GetTransform(0)->Position(&position);
		kachujin2->GetTrasnform(0)->Position(&position);

		if (Keyboard::Get()->Press('L'))
			position.x += 20 * Time::Delta();
		else if (Keyboard::Get()->Press('J'))
			position.x -= 20 * Time::Delta();

		if (Keyboard::Get()->Press('I'))
			position.z += 20 * Time::Delta();
		else if (Keyboard::Get()->Press('K'))
			position.z -= 20 * Time::Delta();

		if (Keyboard::Get()->Press('O'))
			position.y += 20 * Time::Delta();
		else if (Keyboard::Get()->Press('U'))
			position.y -= 20 * Time::Delta();

		/*sphere2->GetTransform(0)->Position(position);
		sphere2->UpdateSubResource();*/
		kachujin2->GetTrasnform(0)->Position(position);
		kachujin2->UpdateSubResource();
	}

	sky->Update();

	plane->Update();
	cube->Update();
	cylinder->Update();
	sphere->Update();
	sphere2->Update();

	airplane->Update();
	kachujin->Update();
	kachujin2->Update();

	Matrix worlds[MAX_MODEL_TRANSFORMS];
	for (UINT i = 0; i < kachujin->GetTransformCount(); i++)
	{
		kachujin->GetAttachBones(i, worlds);
		weapon->GetTransform(i)->World(weaponInitTransform->World() * worlds[40]);
	}

	
	weapon->UpdateSubResource();
	weapon->Update();
}

void EnvCubeDemo::PreRender()
{
	Vector3 p, s;
	//sphere2->GetTransform(0)->Position(&p);
	//sphere2->GetTransform(0)->Scale(&s);
	kachujin2->GetTrasnform(0)->Position(&p);
	kachujin2->GetTrasnform(0)->Scale(&s);
	
	envCube->PreRender(p, s);
	{
		sky->Pass(0);
		sky->Render();

		Pass(1);

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

	
}

void EnvCubeDemo::Render()
{
	sky->Pass(4);
	sky->Render();

	Pass(5);

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

	envCube->Render();
	wall->Render();
	sphere2->Pass(8);
	sphere2->Render();
	kachujin2->Pass(10);
	kachujin2->Render();
}

void EnvCubeDemo::Mesh()
{
	//Create Materiald
	{
		floor = new Material(shader);
		floor->DiffuseMap("Floor.png");
		floor->SpecularMap("Floor_Specular.png");
		floor->NormalMap("Floor_Normal.png");
		floor->Specular(1, 1, 1, 20);

		stone = new Material(shader);
		stone->DiffuseMap("Stones.png");
		stone->SpecularMap("Stones_Specular.png");
		stone->NormalMap("Stones_Normal.png");
		stone->Specular(1, 1, 1, 20);
		stone->Emissive(0.15f, 0.15f, 0.15f, 0.3f);

		brick = new Material(shader);
		brick->DiffuseMap("Bricks.png");
		brick->SpecularMap("Bricks_Specular.png");
		brick->NormalMap("Bricks_Normal.png");
		brick->Specular(1, 1, 1, 20);
		brick->Emissive(0.15f, 0.15f, 0.15f, 0.3f);

		wall = new Material(shader);
		wall->DiffuseMap("Wall.png");
		wall->SpecularMap("Wall_Specular.png");
		wall->NormalMap("Wall_Normal.png");
		wall->Specular(1, 1, 1, 20);
		wall->Emissive(0.15f, 0.15f, 0.15f, 0.3f);
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

		sphere2 = new MeshRender(shader, new MeshSphere(0.5f, 20, 20));
		transform = sphere2->AddTransform();
		transform->Position(0, 20, 0);
		transform->Scale(5, 5, 5);
	}

	cube->UpdateSubResource();
	plane->UpdateSubResource();
	cylinder->UpdateSubResource();
	sphere->UpdateSubResource();
	sphere2->UpdateSubResource();

	meshes.push_back(cube);
	meshes.push_back(plane);
	meshes.push_back(cylinder);
	meshes.push_back(sphere);
	meshes.push_back(sphere2);
}

void EnvCubeDemo::Airplane()
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

void EnvCubeDemo::Kachujin()
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

void EnvCubeDemo::Kachujin2()
{
	kachujin2 = new ModelAnimator(shader);
	kachujin2->ReadMesh(L"Kachujin/Mesh");
	kachujin2->ReadMaterial(L"Kachujin/Mesh");
	kachujin2->ReadClip(L"Kachujin/Idle");
	kachujin2->ReadClip(L"Kachujin/Walk");
	kachujin2->ReadClip(L"Kachujin/Run");
	kachujin2->ReadClip(L"Kachujin/Slash");
	kachujin2->ReadClip(L"Kachujin/Uprock");

	Transform* transform = nullptr;

	transform = kachujin2->AddTransform();
	transform->Position(0, 0, -40);
	transform->Scale(0.075f, 0.075f, 0.075f);
	kachujin2->PlayTweenMode(0, 4, 1.0f);

	kachujin2->UpdateSubResource();

	animators.push_back(kachujin2);
}

void EnvCubeDemo::Weapon()
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

void EnvCubeDemo::PointLights()
{
	PointLight light;
	light =
	{
		Color(0.0f, 0.0f, 0.0f, 1.0f), //Ambient
		Color(0.0f, 0.3f, 1.0f, 1.0f), //Diffuse
		Color(0.0f, 0.0f, 0.7f, 1.0f), //Specular
		Color(0.0f, 0.0f, 0.7f, 1.0f), //Emissive
		Vector3(-30, 10, -30), 15.0f, 0.9f
	};
	Lighting::Get()->AddPointLight(light);

	light =
	{
		Color(0.0f, 0.0f, 0.0f, 1.0f),
		Color(1.0f, 0.0f, 0.0f, 1.0f),
		Color(0.6f, 0.2f, 0.0f, 1.0f),
		Color(0.6f, 0.3f, 0.0f, 1.0f),
		Vector3(15, 10, -30), 10.0f, 0.3f
	};
	Lighting::Get()->AddPointLight(light);

	light =
	{
		Color(0.0f, 0.0f, 0.0f, 1.0f), //Ambient
		Color(0.0f, 1.0f, 0.0f, 1.0f), //Diffuse
		Color(0.0f, 0.7f, 0.0f, 1.0f), //Specular
		Color(0.0f, 0.7f, 0.0f, 1.0f), //Emissive
		Vector3(-5, 1, -17.5f), 5.0f, 0.9f
	};
	Lighting::Get()->AddPointLight(light);

	light =
	{
		Color(0.0f, 0.0f, 0.0f, 1.0f),
		Color(0.0f, 0.0f, 1.0f, 1.0f),
		Color(0.0f, 0.0f, 0.7f, 1.0f),
		Color(0.0f, 0.0f, 0.7f, 1.0f),
		Vector3(-10, 1, -17.5f), 5.0f, 0.9f
	};
	Lighting::Get()->AddPointLight(light);
}

void EnvCubeDemo::SpotLights()
{
	SpotLight light;
	light =
	{
		Color(0.3f, 1.0f, 0.0f, 1.0f),
		Color(0.7f, 1.0f, 0.0f, 1.0f),
		Color(0.3f, 1.0f, 0.0f, 1.0f),
		Color(0.3f, 1.0f, 0.0f, 1.0f),
		Vector3(-15, 20, -30), 25.0f,
		Vector3(0, -1, 0), 30.0f, 0.9f
	};
	Lighting::Get()->AddSpotLight(light);

	light =
	{
		Color(1.0f, 0.2f, 0.9f, 1.0f),
		Color(1.0f, 0.2f, 0.9f, 1.0f),
		Color(1.0f, 0.2f, 0.9f, 1.0f),
		Color(1.0f, 0.2f, 0.9f, 1.0f),
		Vector3(0, 20, -30), 30.0f,
		Vector3(0, -1, 0), 40.0f, 0.9f
	};
	Lighting::Get()->AddSpotLight(light);
}

void EnvCubeDemo::Pass(UINT val)
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
