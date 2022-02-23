#include "stdafx.h"
#include "FrustumDemo.h"

void FrustumDemo::Initialize()
{
	//¹Ù´Ú
	gridShader = new Shader(L"26_Lighting.fxo");

	floor = new Material(gridShader);
	floor->DiffuseMap(L"White.png");

	plane = new MeshRender(gridShader, new MeshPlane());
	plane->AddTransform()->Scale(10, 10, 10);
	plane->UpdateSubResource();
	plane->Pass(0);

	perspective = new Perspective(1024, 768, 1, zFar, Math::PI * fov);
	frustum = new Frustum(nullptr, perspective);
	
	//Å¥ºê
	shader = new Shader(L"12_Mesh.fxo");
	perFrame = new PerFrame(shader);

	red = new Material(shader);
	red->DiffuseMap("Red.png");

	for (float z = -50.0f; z <= 50.0f; z += 10)
	{
		for (float y = -50.0f; y <= 50.0f; y += 10)
		{
			for (float x = -50.0f; x <= 50.0f; x += 10)
			{
				Transform* transform = new Transform(shader);
				transform->Position(x, y, z);

				transforms.push_back(transform);
			}
		}
	}

	CreateMesh();
}

void FrustumDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(red);
	SafeDelete(vertexBuffer);
	SafeDelete(indexBuffer);

	SafeDelete(gridShader);
	SafeDelete(floor);
	SafeDelete(plane);

	SafeDelete(camera);
	SafeDelete(perspective);
	SafeDelete(frustum);

	for (Transform* transform : transforms)
		SafeDelete(transform);
	SafeDelete(perFrame);
}

void FrustumDemo::Update()
{
	ImGui::InputFloat("Far", &zFar, 1.0f);
	ImGui::InputFloat("FOV", &fov, 1e-3f);
	perspective->Set(1024, 768, 1, zFar, Math::PI * fov);

	frustum->Update();

	perFrame->Update();
	plane->Update();
}

void FrustumDemo::Render()
{
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	floor->Render();
	plane->Render();

	perFrame->Render();
	vertexBuffer->Render();
	indexBuffer->Render();

	red->Render();

	UINT drawCount = 0; //µð¹ö±ë¿ë
	Vector3 position;
	for (Transform* transform : transforms)
	{
		transform->Position(&position);

		if (frustum->CheckPoint(position))
		{
			transform->Render();
			shader->DrawIndexed(0, 0, 36);

			drawCount++;
		}
	}

	string str = to_string(drawCount) + " / " + to_string(transforms.size());
	Gui::Get()->RenderText(10, 60, 1, 0, 0, str);
}

void FrustumDemo::CreateMesh()
{
	vector<Mesh::VertexMesh> v;

	float w = 0.5f;
	float h = 0.5f;
	float d = 0.5f;

	//Front
	v.push_back(Mesh::VertexMesh(-w, -h, -d, 0, 1, 0, 0, -1, 1, 0, 0));
	v.push_back(Mesh::VertexMesh(-w, +h, -d, 0, 0, 0, 0, -1, 1, 0, 0));
	v.push_back(Mesh::VertexMesh(+w, +h, -d, 1, 0, 0, 0, -1, 1, 0, 0));
	v.push_back(Mesh::VertexMesh(+w, -h, -d, 1, 1, 0, 0, -1, 1, 0, 0));

	//Back
	v.push_back(Mesh::VertexMesh(-w, -h, +d, 1, 1, 0, 0, 1, -1, 0, 0));
	v.push_back(Mesh::VertexMesh(+w, -h, +d, 0, 1, 0, 0, 1, -1, 0, 0));
	v.push_back(Mesh::VertexMesh(+w, +h, +d, 0, 0, 0, 0, 1, -1, 0, 0));
	v.push_back(Mesh::VertexMesh(-w, +h, +d, 1, 0, 0, 0, 1, -1, 0, 0));

	//Top
	v.push_back(Mesh::VertexMesh(-w, +h, -d, 0, 1, 0, 1, 0, 1, 0, 0));
	v.push_back(Mesh::VertexMesh(-w, +h, +d, 0, 0, 0, 1, 0, 1, 0, 0));
	v.push_back(Mesh::VertexMesh(+w, +h, +d, 1, 0, 0, 1, 0, 1, 0, 0));
	v.push_back(Mesh::VertexMesh(+w, +h, -d, 1, 1, 0, 1, 0, 1, 0, 0));

	//Bottom
	v.push_back(Mesh::VertexMesh(-w, -h, -d, 1, 1, 0, -1, 0, -1, 0, 0));
	v.push_back(Mesh::VertexMesh(+w, -h, -d, 0, 1, 0, -1, 0, -1, 0, 0));
	v.push_back(Mesh::VertexMesh(+w, -h, +d, 0, 0, 0, -1, 0, -1, 0, 0));
	v.push_back(Mesh::VertexMesh(-w, -h, +d, 1, 0, 0, -1, 0, -1, 0, 0));

	//Left
	v.push_back(Mesh::VertexMesh(-w, -h, +d, 0, 1, -1, 0, 0, 0, 0, -1));
	v.push_back(Mesh::VertexMesh(-w, +h, +d, 0, 0, -1, 0, 0, 0, 0, -1));
	v.push_back(Mesh::VertexMesh(-w, +h, -d, 1, 0, -1, 0, 0, 0, 0, -1));
	v.push_back(Mesh::VertexMesh(-w, -h, -d, 1, 1, -1, 0, 0, 0, 0, -1));

	//Right
	v.push_back(Mesh::VertexMesh(+w, -h, -d, 0, 1, 1, 0, 0, 0, 0, 1));
	v.push_back(Mesh::VertexMesh(+w, +h, -d, 0, 0, 1, 0, 0, 0, 0, 1));
	v.push_back(Mesh::VertexMesh(+w, +h, +d, 1, 0, 1, 0, 0, 0, 0, 1));
	v.push_back(Mesh::VertexMesh(+w, -h, +d, 1, 1, 1, 0, 0, 0, 0, 1));


	Mesh::VertexMesh* vertices = new Mesh::VertexMesh[v.size()];
	UINT vertexCount = v.size();

	copy
	(
		v.begin(), v.end(),
		stdext::checked_array_iterator<Mesh::VertexMesh *>(vertices, vertexCount)
	);

	UINT indexCount = 36;
	UINT* indices = new UINT[indexCount]
	{
		0, 1, 2, 0, 2, 3,
		4, 5, 6, 4, 6, 7,
		8, 9, 10, 8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 21, 22, 20, 22, 23
	};


	vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(Mesh::VertexMesh));
	indexBuffer = new IndexBuffer(indices, indexCount);

	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);
}
