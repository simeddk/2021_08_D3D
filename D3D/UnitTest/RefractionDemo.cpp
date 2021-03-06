#include "stdafx.h"
#include "RefractionDemo.h"

void RefractionDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(21, 0, 0);
	Context::Get()->GetCamera()->Position(126, 35, 39);
	((Freedom*)Context::Get()->GetCamera())->Speed(50, 2);
	
	shader = new Shader(L"50_Refraction.fxo");
	shadow = new Shadow(shader, Vector3(128, 0, 128), 128);
	
	sky = new Sky(shader);

	terrain = new Terrain(shader, L"Terrain/Gray256.png");
	terrain->BaseMap(L"Terrain/Cliff (Layered Rock).jpg");
	terrain->NormalMap(L"Terrain/Cliff (Layered Rock)_NormalMap.png");
	
	Mesh();
	Airplane();
	Kachujin();
	Weapon();

	PointLights();
	SpotLights();

	refraction = new Refraction(shader, mirrorPlane->GetTransform(0), L"Environment/Wave1.dds");
}

void RefractionDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(sky);

	//Materials
	SafeDelete(floor);
	SafeDelete(stone);
	SafeDelete(brick);
	SafeDelete(wall);

	//Mesh
	SafeDelete(cube);
	SafeDelete(cylinder);
	SafeDelete(sphere);

	//Mesh
	SafeDelete(airplane);
	SafeDelete(kachujin);
	SafeDelete(weapon);

	SafeDelete(shadow);
	SafeDelete(terrain);

	SafeDelete(mirrorPlane);
	SafeDelete(refraction);
}

void RefractionDemo::Update()
{
	//Scaterring Test
	{
		static bool bRealTime = false;
		static float speed = 1.0f;
		ImGui::Checkbox("RealTime", &bRealTime);
		ImGui::SliderFloat("Speed", &speed, 0.5f, 3.0f);
		sky->RealTime(bRealTime, speed);
	}

	//Fog Test
	{
		ImGui::Separator();
		static UINT type = Lighting::Get()->FogType();
		ImGui::InputInt("Fog Type", (int*)&type);
		type %= 3;
		Lighting::Get()->FogType() = type;

		ImGui::ColorEdit3("Fog Color", Lighting::Get()->FogColor());
		ImGui::SliderFloat2("Fog Distance", Lighting::Get()->FogDistance(), 1, 200);
		ImGui::SliderFloat("Fog Density", &Lighting::Get()->FogDensity(), 0, 5);
		ImGui::Separator();
	}

	//물결 텍스쳐 변경 테스트
	static bool bUseOpenFile = true;
	ImGui::Checkbox("Use FileDialog", &bUseOpenFile);
	if (bUseOpenFile)
	{
		if (ImGui::Button("NormalMap"))
		{
			wstring filePath = L"../../_Textures/Environment/";
			const WCHAR* waveFilter = L"Wave\0wave*.dds";
			Path::OpenFileDialog(L"", waveFilter, filePath, refraction->OnButtonPressed_NormalMap);


		}
	}
	else //라디오 버튼 방식
	{
		static UINT item = 0;
		const WCHAR* waveFiles[] =
		{
			L"Environment/Wave.dds",
			L"Environment/Wave1.dds",
			L"Environment/Wave2.dds"
		};
		ImGui::RadioButton("Wave", (int*)&item, 0); ImGui::SameLine();
		ImGui::RadioButton("Wave1", (int*)&item, 1); ImGui::SameLine();
		ImGui::RadioButton("Wave2", (int*)&item, 2);
		refraction->NormalMap(waveFiles[item]);
	}

	sky->Update();

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

	terrain->Update();

	refraction->Update();
	mirrorPlane->Update();
}

void RefractionDemo::PreRender()
{
	//Shadow
	shadow->PreRender();
	{
		Pass(0);

		wall->Render();
		sphere->Render();

		brick->Render();
		cylinder->Render();

		stone->Render();
		cube->Render();

		airplane->Render();
		kachujin->Render();
		weapon->Render();

		terrain->Pass(3);
		terrain->Render();
	}

	//Refraction
	refraction->PreRender();
	{
		sky->Pass(8);
		sky->Render();

		Pass(4);

		wall->Render();
		sphere->Render();

		brick->Render();
		cylinder->Render();

		stone->Render();
		cube->Render();

		floor->Render();

		airplane->Render();
		kachujin->Render();
		weapon->Render();

		terrain->Pass(7);
		terrain->Render();

		Context::Get()->Clipping() = Plane(0, 0, 0, 0);

	}

	sky->PreRender();
}

void RefractionDemo::Render()
{
	sky->Pass(8);
	sky->Render();

	Pass(4);

	wall->Render();
	sphere->Render();

	brick->Render();
	cylinder->Render();

	stone->Render();
	cube->Render();

	floor->Render();

	airplane->Render();
	kachujin->Render();
	weapon->Render();

	terrain->Pass(7);
	terrain->Render();

	refraction->Render();
	mirrorPlane->Pass(11);
	mirrorPlane->Render();
}

void RefractionDemo::PostRender()
{
	sky->PostRender();
}

void RefractionDemo::Mesh()
{
	//Create Material
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
		transform->Position(128, 0, 128); //<- 위치 수정
		transform->Scale(20, 10, 20);
		SetTransform(transform);  //<- 추가

		cylinder = new MeshRender(shader, new MeshCylinder(0.3f, 0.5f, 3.0f, 20, 20));
		sphere = new MeshRender(shader, new MeshSphere(0.5f, 20, 20));
		for (UINT i = 0; i < 5; i++)
		{
			transform = cylinder->AddTransform();
			transform->Position(128 - 30, 3, 128 - 15.0f + (float)i * 15.0f); //<- 위치 수정
			transform->Scale(5, 5, 5);
			SetTransform(transform); //추가

			transform = cylinder->AddTransform();
			transform->Position(128 + 30, 3, 128 - 15.0f + (float)i * 15.0f); //<- 위치 수정
			transform->Scale(5, 5, 5);
			SetTransform(transform); //<- 추가

			transform = sphere->AddTransform();
			transform->Position(128 - 30, 12.5f, 128 - 15.0f + (float)i * 15.0f); //<- 위치 수정
			transform->Scale(5, 5, 5);
			SetTransform(transform); //<- 추가

			transform = sphere->AddTransform();
			transform->Position(128 + 30, 12.5f, 128 - 15.0f + (float)i * 15.0f); //<- 위치 수정
			transform->Scale(5, 5, 5);
			SetTransform(transform); //<- 추가
		}

		mirrorPlane = new MeshRender(shader, new MeshPlane());
		transform = mirrorPlane->AddTransform();
		transform->Position(128, 3, 128);
		transform->Scale(20, 1, 20);
	}

	cube->UpdateSubResource();
	cylinder->UpdateSubResource();
	sphere->UpdateSubResource();
	mirrorPlane->UpdateSubResource();

	meshes.push_back(cube);
	meshes.push_back(cylinder);
	meshes.push_back(sphere);
}

void RefractionDemo::Airplane()
{
	airplane = new ModelRender(shader);
	airplane->ReadMesh(L"B787/Airplane");
	airplane->ReadMaterial(L"B787/Airplane");

	Transform* transform = airplane->AddTransform();
	transform->Position(128 + 2.0f, 9.91f, 128 + 2.0f);
	transform->Scale(Vector3(0.004f, 0.004f, 0.004f));
	SetTransform(transform);
	airplane->UpdateSubResource();

	models.push_back(airplane);
}

void RefractionDemo::Kachujin()
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
	transform->Position(128 + 0, 0, 128 - 30); //<- 수정
	transform->Scale(0.075f, 0.075f, 0.075f);
	SetTransform(transform); //<- 추가
	kachujin->PlayTweenMode(0, 0, 1.0f);

	transform = kachujin->AddTransform();
	transform->Position(128 - 15, 0, 128 - 30); //<- 수정
	transform->Scale(0.075f, 0.075f, 0.075f);
	SetTransform(transform); //<- 추가
	kachujin->PlayTweenMode(1, 1, 1.0f);

	transform = kachujin->AddTransform();
	transform->Position(128 - 30, 0, 128 - 30); //<- 수정
	transform->Scale(0.075f, 0.075f, 0.075f);
	SetTransform(transform); //<- 추가
	kachujin->PlayTweenMode(2, 2, 0.75f);

	transform = kachujin->AddTransform();
	transform->Position(128 + 15, 0, 128 - 30); //<- 수정
	transform->Scale(0.075f, 0.075f, 0.075f);
	SetTransform(transform); //<- 추가
	kachujin->PlayBlendMode(3, 0, 1, 2);
	kachujin->SetBlendAlpha(3, 1.75f);

	transform = kachujin->AddTransform();
	transform->Position(128 + 30, 0, 128 - 32.5f); //<-- 수정
	transform->Scale(0.075f, 0.075f, 0.075f);
	SetTransform(transform); //<- 추가
	kachujin->PlayTweenMode(4, 4, 0.75f);

	kachujin->UpdateSubResource();

	animators.push_back(kachujin);

}

void RefractionDemo::Weapon()
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

void RefractionDemo::PointLights()
{
	PointLight light;
	light =
	{
		Color(0.0f, 0.0f, 0.0f, 1.0f), //Ambient
		Color(0.0f, 0.3f, 1.0f, 1.0f), //Diffuse
		Color(0.0f, 0.0f, 0.7f, 1.0f), //Specular
		Color(0.0f, 0.0f, 0.7f, 1.0f), //Emissive
		Vector3(128 - 30, 10, 128 - 30), 15.0f, 0.9f
	};
	Lighting::Get()->AddPointLight(light);

	light =
	{
		Color(0.0f, 0.0f, 0.0f, 1.0f),
		Color(1.0f, 0.0f, 0.0f, 1.0f),
		Color(0.6f, 0.2f, 0.0f, 1.0f),
		Color(0.6f, 0.3f, 0.0f, 1.0f),
		Vector3(128 + 15, 10, 128 - 30), 10.0f, 0.3f
	};
	Lighting::Get()->AddPointLight(light);

	light =
	{
		Color(0.0f, 0.0f, 0.0f, 1.0f), //Ambient
		Color(0.0f, 1.0f, 0.0f, 1.0f), //Diffuse
		Color(0.0f, 0.7f, 0.0f, 1.0f), //Specular
		Color(0.0f, 0.7f, 0.0f, 1.0f), //Emissive
		Vector3(128 - 5, 1, 128 - 17.5f), 5.0f, 0.9f
	};
	Lighting::Get()->AddPointLight(light);

	light =
	{
		Color(0.0f, 0.0f, 0.0f, 1.0f),
		Color(0.0f, 0.0f, 1.0f, 1.0f),
		Color(0.0f, 0.0f, 0.7f, 1.0f),
		Color(0.0f, 0.0f, 0.7f, 1.0f),
		Vector3(128 - 10, 1, 128 - 17.5f), 5.0f, 0.9f
	};
	Lighting::Get()->AddPointLight(light);
}

void RefractionDemo::SpotLights()
{
	SpotLight light;
	light =
	{
		Color(0.3f, 1.0f, 0.0f, 1.0f),
		Color(0.7f, 1.0f, 0.0f, 1.0f),
		Color(0.3f, 1.0f, 0.0f, 1.0f),
		Color(0.3f, 1.0f, 0.0f, 1.0f),
		Vector3(128 - 15, 20, 128 - 30), 25.0f,
		Vector3(0, -1, 0), 30.0f, 0.9f
	};
	Lighting::Get()->AddSpotLight(light);

	light =
	{
		Color(1.0f, 0.2f, 0.9f, 1.0f),
		Color(1.0f, 0.2f, 0.9f, 1.0f),
		Color(1.0f, 0.2f, 0.9f, 1.0f),
		Color(1.0f, 0.2f, 0.9f, 1.0f),
		Vector3(128 + 0, 20, 128 - 30), 30.0f,
		Vector3(0, -1, 0), 40.0f, 0.9f
	};
	Lighting::Get()->AddSpotLight(light);
}

void RefractionDemo::Pass(UINT val)
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

void RefractionDemo::SetTransform(Transform * transform)
{
	//해당 물체의 위치, 크기 얻어오기
	Vector3 position;
	transform->Position(&position);

	Vector3 scale;
	transform->Scale(&scale);

	//지면의 높이만큼 올려주기
	float y = terrain->GetHeight(position);
	position.y += y + scale.y * 0.5f;
	transform->Position(position);
}
