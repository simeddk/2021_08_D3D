#include "stdafx.h"
#include "BloomDemo.h"

void BloomDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(20, 0, 0);
	Context::Get()->GetCamera()->Position(1, 36, -85);
	((Freedom*)Context::Get()->GetCamera())->Speed(50, 2);
	
	shader = new Shader(L"33_Billboard.fxo");
	
	float w = D3D::Width(), h = D3D::Height();
	for (UINT i = 0 ; i < 6; i++)
		renderTarget[i] = new RenderTarget(w, h);

	depthStencil = new DepthStencil(w, h);
	viewport = new Viewport(w, h);

	render2D = new Render2D();
	render2D->GetTransform()->Position(200, 120, 0);
	render2D->GetTransform()->Scale(355, 200, 1);
	render2D->SRV(renderTarget[0]->SRV());

	sky = new CubeSky(L"Environment/SunsetCube1024.dds");

	postProcess = new PostProcess(L"37_Bloom.fxo");
	

	Mesh();
	Airplane();
	Kachujin();
	Weapon();

	PointLights();
	SpotLights();

	Billboards();
}

void BloomDemo::Destroy()
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

	SafeDelete(billboard);
	
	for (UINT i = 0 ; i < 6; i++)
		SafeDelete(renderTarget[i]);

	SafeDelete(depthStencil);
	SafeDelete(viewport);

	SafeDelete(postProcess);
	SafeDelete(render2D);
}

void BloomDemo::Update()
{
	//램버트 테스트
	ImGui::SliderFloat3("LightDirection", Lighting::Get()->Direction(), -1, +1);

	//가우시안블러 테스트
	{
		Vector2 pixelSize = Vector2(1 / D3D::Width(), 1 / D3D::Height());
		postProcess->GetShader()->AsVector("PixelSize")->SetFloatVector(pixelSize);

		ImGui::SliderFloat("Threshold", &threshold, 0.1f, 0.999f);
		postProcess->GetShader()->AsScalar("Threshold")->SetFloat(threshold);
	}

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

	postProcess->Update();
	render2D->Update();
	billboard->Update();
}

void BloomDemo::PreRender()
{
	viewport->RSSetViewport();

	//Diffuse
	renderTarget[0]->PreRender(depthStencil);
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

		billboard->Render();
	}

	//Luminosity
	{
		renderTarget[1]->PreRender(depthStencil);

		postProcess->Pass(1);
		postProcess->SRV(renderTarget[0]->SRV());
		postProcess->Render();
	}

	//GaussianBlurX
	{
		renderTarget[2]->PreRender(depthStencil);
		postProcess->Pass(2);
		postProcess->SRV(renderTarget[0]->SRV());
		postProcess->Render();
	}

	//GaussianBlurY
	{
		renderTarget[3]->PreRender(depthStencil);
		postProcess->Pass(3);
		postProcess->SRV(renderTarget[2]->SRV());
		postProcess->Render();
	}

	//Composite
	{
		static float BlurRatio = 0.5f;
		ImGui::SliderFloat("BlurRatio", &BlurRatio, 0.0f, 1.0f);
		postProcess->GetShader()->AsScalar("BlurRatio")->SetFloat(BlurRatio);

		postProcess->GetShader()->AsSRV("LuminosityMap")->SetResource(renderTarget[1]->SRV());
		postProcess->GetShader()->AsSRV("BlurMap")->SetResource(renderTarget[3]->SRV());

		renderTarget[4]->PreRender(depthStencil);
		postProcess->Pass(4);
		postProcess->SRV(renderTarget[0]->SRV());
		postProcess->Render();
	}

	//ColorGrading
	{
		ImGui::Separator();

		static float Grading = 0.5f;
		ImGui::InputFloat("Grading", &Grading, 0.01f);
		postProcess->GetShader()->AsScalar("Grading")->SetFloat(Grading);

		static float Correlation = 0.5f;
		ImGui::InputFloat("Correlation", &Correlation, 0.01f);
		postProcess->GetShader()->AsScalar("Correlation")->SetFloat(Correlation);

		static float Concentration = 0.5f;
		ImGui::InputFloat("Concentration", &Concentration, 0.01f);
		postProcess->GetShader()->AsScalar("Concentration")->SetFloat(Concentration);

		ImGui::Separator();

		renderTarget[5]->PreRender(depthStencil);
		postProcess->Pass(5);
		postProcess->SRV(renderTarget[4]->SRV());
		postProcess->Render();
	}
	
}

void BloomDemo::Render()
{
	static UINT rtvIndex = 5;
	UINT indexCount = ARRAYSIZE(renderTarget);
	ImGui::InputInt("RTV Index", (int*)&rtvIndex);
	rtvIndex %= indexCount;

	postProcess->Pass(0);
	postProcess->SRV(renderTarget[rtvIndex]->SRV());

	postProcess->Render();
	render2D->Render();
}

void BloomDemo::Mesh()
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

void BloomDemo::Airplane()
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

void BloomDemo::Kachujin()
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

void BloomDemo::Weapon()
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

void BloomDemo::PointLights()
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

void BloomDemo::SpotLights()
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

void BloomDemo::Billboards()
{
	billboard = new Billboard(shader);
	billboard->Pass(9);

	billboard->AddTexture(L"Terrain/grass_01.tga");
	billboard->AddTexture(L"Terrain/grass_07.tga");
	billboard->AddTexture(L"Terrain/grass_08.tga");

	for (UINT i = 0; i < 1200; i++)
	{
		Vector2 position = Math::RandomVec2(-60.0f, +60.0f);
		Vector2 scale = Math::RandomVec2(3.f, 5.f);

		billboard->Add(Vector3(position.x, scale.y * 0.5f , position.y), scale, 0);
	}

	for (UINT i = 0; i < 1200; i++)
	{
		Vector2 position = Math::RandomVec2(-60.0f, +60.0f);
		Vector2 scale = Math::RandomVec2(3.f, 5.f);

		billboard->Add(Vector3(position.x, scale.y * 0.5f, position.y), scale, 1);
	}

	for (UINT i = 0; i < 1200; i++)
	{
		Vector2 position = Math::RandomVec2(-60.0f, +60.0f);
		Vector2 scale = Math::RandomVec2(3.f, 5.f);

		billboard->Add(Vector3(position.x, scale.y * 0.5f, position.y), scale, 2);
	}
}


void BloomDemo::Pass(UINT val)
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
