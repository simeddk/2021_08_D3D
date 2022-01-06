#include "stdafx.h"
#include "ProjectDemo.h"


void ProjectDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(20, 0, 0);
	Context::Get()->GetCamera()->Position(1, 36, -85);

	shader = new Shader(L"12_Mesh.fxo");
	
	sky = new CubeSky(L"Environment/SnowCube1024.dds");

	CreateMesh();

	sDirection = shader->AsVector("LightDirection");

	cubeMapShader = new Shader(L"13_CubeMap.fxo");
	cubeMap = new CubeMap(cubeMapShader);
	cubeMap->Texture(L"Environment/SnowCube1024.dds");
	cubeMap->GetTransform()->Position(0, 20, 0);
	cubeMap->GetTransform()->Scale(10, 10, 10);

	marker = new MeshCylinder(shader, 0.5f, 0.1f, 1.0f, 2.5f, 15.0f);
	marker->DiffuseMap(L"Red.png");
	marker->GetTransform()->Position(0, 15, 0);
}

void ProjectDemo::Destroy()
{
	SafeDelete(shader);

	SafeDelete(quad);
	SafeDelete(plane);
	SafeDelete(cube);
	SafeDelete(marker);

	for (UINT i = 0; i < 10; i++)
	{
		SafeDelete(cylinders[i]);
		SafeDelete(spheres[i]);
	}

	SafeDelete(cubeMapShader);
	SafeDelete(cubeMap);

	SafeDelete(sky);
}

void ProjectDemo::Update()
{
	//뷰포트 테스트
	{
		static float topLeftX = 0;
		static float topLeftY = 0;
		static float width = D3D::Width();
		static float height = D3D::Height();

		ImGui::SliderFloat("X", &topLeftX, -640, 640);
		ImGui::SliderFloat("Y", &topLeftY, -360, 360);
		ImGui::SliderFloat("Width", &width, 0, D3D::Width() * 2);
		ImGui::SliderFloat("Height", &height, 0, D3D::Height() * 2);

		Viewport* Vp = Context::Get()->GetViewport();
		Vp->Set(width, height, topLeftX, topLeftY);
	}

	//Marker 이동
	{
		Vector3 P;
		marker->GetTransform()->Position(&P);

		if (Keyboard::Get()->Press(VK_UP))
			P += Context::Get()->GetCamera()->Forward() * 20.0f * Time::Delta();
		else if (Keyboard::Get()->Press(VK_DOWN))
			P -= Context::Get()->GetCamera()->Forward() * 20.0f * Time::Delta();

		if (Keyboard::Get()->Press(VK_RIGHT))
			P += Context::Get()->GetCamera()->Right() * 20.0f * Time::Delta();
		else if (Keyboard::Get()->Press(VK_LEFT))
			P -= Context::Get()->GetCamera()->Right() * 20.0f * Time::Delta();

		P.y = 15.0f;
		marker->GetTransform()->Position(P);
	}

	//조명 방향 테스트
	ImGui::SliderFloat3("Direction", direction, -1, +1);
	sDirection->SetFloatVector(direction);

	sky->Update();
	
	quad->Update();
	plane->Update();

	cube->Update();

	for (UINT i = 0; i < 10; i++)
	{
		cylinders[i]->Update();
		spheres[i]->Update();
	}

	cubeMap->Update();
	marker->Update();
}

void ProjectDemo::Render()
{
	static bool bWireframe = false;
	ImGui::Checkbox("Wire Frame", &bWireframe);

	quad->Pass(bWireframe ? 1 : 0);
	plane->Pass(bWireframe ? 1 : 0);
	cube->Pass(bWireframe ? 1 : 0);

	//Project Text
	{
		Transform transform;
		transform.World(marker->GetTransform()->World());

		Vector3 temp;
		transform.Position(&temp);
		temp.y += 3;
		transform.Position(temp);

		Matrix W, V, P;
		W = transform.World();
		V = Context::Get()->View();
		P = Context::Get()->Projection();

		Vector3 vpPosition = Context::Get()->GetViewport()->Project(Vector3(0, 0, 0), W, V, P);
		Vector3 wPosition = Vector3(transform.World()._41, transform.World()._42, transform.World()._43);

		string str = "VP : ";
		str += to_string((int)vpPosition.x) + ", " + to_string((int)vpPosition.y) + ", " + to_string((int)vpPosition.z);
		str += "\nWP : ";
		str += to_string((int)wPosition.x) + ", " + to_string((int)wPosition.y) + ", " + to_string((int)wPosition.z);

		Gui::Get()->RenderText(Vector2(vpPosition.x, vpPosition.y), Color(1, 0, 0, 1), str);
	}

	sky->Render();

	quad->Render();

	for (UINT i = 0; i < 10; i++)
	{
		cylinders[i]->Pass(bWireframe ? 1 : 0);
		cylinders[i]->Render();

		spheres[i]->Pass(bWireframe ? 1 : 0);
		spheres[i]->Render();
	}

	

	cube->Render();
	plane->Render();
	
	//cubeMap->Render();

	marker->Render();
}

void ProjectDemo::CreateMesh()
{
	quad = new MeshQuad(shader);
	quad->DiffuseMap(L"Box.png");

	plane = new MeshPlane(shader, 2.5f, 2.5f);
	plane->GetTransform()->Scale(12, 1, 12);
	plane->DiffuseMap(L"Floor.png");

	cube = new MeshCube(shader);
	cube->GetTransform()->Position(0, 5, 0);
	cube->GetTransform()->Scale(20, 10, 20);
	cube->DiffuseMap(L"Stones.png");

	for (UINT i = 0; i < 5; i++)
	{
		//Cylinders
		cylinders[i * 2 + 0] = new MeshCylinder(shader, 0.3f, 0.5f, 3.0f, 20, 20);
		cylinders[i * 2 + 0]->GetTransform()->Position(-30, 6, (float)i * 15.0f - 15.0f);
		cylinders[i * 2 + 0]->GetTransform()->Scale(5, 5, 5);
		cylinders[i * 2 + 0]->DiffuseMap(L"Bricks.png");

		cylinders[i * 2 + 1] = new MeshCylinder(shader, 0.3f, 0.5f, 3.0f, 20, 20);
		cylinders[i * 2 + 1]->GetTransform()->Position(+30, 6, (float)i * 15.0f - 15.0f);
		cylinders[i * 2 + 1]->GetTransform()->Scale(5, 5, 5);
		cylinders[i * 2 + 1]->DiffuseMap(L"Bricks.png");

		//Spheres
		spheres[i * 2 + 0] = new MeshSphere(shader, 0.5f);
		spheres[i * 2 + 0]->GetTransform()->Position(-30, 15.5f, (float)i * 15.0f - 15.0f);
		spheres[i * 2 + 0]->GetTransform()->Scale(5, 5, 5);
		spheres[i * 2 + 0]->DiffuseMap(L"Wall.png");

		spheres[i * 2 + 1] = new MeshSphere(shader, 0.5f);
		spheres[i * 2 + 1]->GetTransform()->Position(+30, 15.5f, (float)i * 15.0f - 15.0f);
		spheres[i * 2 + 1]->GetTransform()->Scale(5, 5, 5);
		spheres[i * 2 + 1]->DiffuseMap(L"Wall.png");
	}
}


