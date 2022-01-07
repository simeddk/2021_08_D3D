#include "stdafx.h"
#include "NoInstanceDemo.h"

void NoInstanceDemo::Initialize()
{
	shader = new Shader(L"22_NoInstance.fxo");
	perFrame = new PerFrame(shader);
	material = new Material(shader);
	material->DiffuseMap(L"Box.png");

	for (UINT i = 0; i < MAX_DRAW_COUNT; i++)
	{
		transforms[i] = new Transform(shader);

		transforms[i]->Position(Math::RandomVec3(-30, 30));
		transforms[i]->Scale(Math::RandomVec3(1.0f, 2.5f));
		transforms[i]->RotationDegree(Math::RandomVec3(-180, 180));
	}

	CreateMesh();
}

void NoInstanceDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(perFrame);
	SafeDelete(material);

	for (UINT i = 0; i < MAX_DRAW_COUNT; i++)
		SafeDelete(transforms[i]);

	SafeDelete(vertexBuffer);
	SafeDelete(indexBuffer);
}

void NoInstanceDemo::Update()
{
	perFrame->Update();

	for (UINT i = 0; i < MAX_DRAW_COUNT; i++)
		transforms[i]->Update();
}

void NoInstanceDemo::Render()
{
	perFrame->Render();
	material->Render();
	
	vertexBuffer->Render();
	indexBuffer->Render();
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (UINT i = 0; i < MAX_DRAW_COUNT; i++)
	{
		transforms[i]->Render();
		shader->DrawIndexed(0, 0, indices.size());
	}
}

void NoInstanceDemo::CreateMesh()
{
	float w = 0.5f;
	float h = 0.5f;
	float d = 0.5f;

	//Front
	vertices.push_back(Mesh::VertexMesh(-w, -h, -d, 0, 1, 0, 0, -1));
	vertices.push_back(Mesh::VertexMesh(-w, +h, -d, 0, 0, 0, 0, -1));
	vertices.push_back(Mesh::VertexMesh(+w, +h, -d, 1, 0, 0, 0, -1));
	vertices.push_back(Mesh::VertexMesh(+w, -h, -d, 1, 1, 0, 0, -1));

	//Back
	vertices.push_back(Mesh::VertexMesh(-w, -h, +d, 1, 1, 0, 0, 1));
	vertices.push_back(Mesh::VertexMesh(+w, -h, +d, 0, 1, 0, 0, 1));
	vertices.push_back(Mesh::VertexMesh(+w, +h, +d, 0, 0, 0, 0, 1));
	vertices.push_back(Mesh::VertexMesh(-w, +h, +d, 1, 0, 0, 0, 1));

	//Top
	vertices.push_back(Mesh::VertexMesh(-w, +h, -d, 0, 1, 0, 1, 0));
	vertices.push_back(Mesh::VertexMesh(-w, +h, +d, 0, 0, 0, 1, 0));
	vertices.push_back(Mesh::VertexMesh(+w, +h, +d, 1, 0, 0, 1, 0));
	vertices.push_back(Mesh::VertexMesh(+w, +h, -d, 1, 1, 0, 1, 0));

	//Bottom
	vertices.push_back(Mesh::VertexMesh(-w, -h, -d, 1, 1, 0, -1, 0));
	vertices.push_back(Mesh::VertexMesh(+w, -h, -d, 0, 1, 0, -1, 0));
	vertices.push_back(Mesh::VertexMesh(+w, -h, +d, 0, 0, 0, -1, 0));
	vertices.push_back(Mesh::VertexMesh(-w, -h, +d, 1, 0, 0, -1, 0));

	//Left
	vertices.push_back(Mesh::VertexMesh(-w, -h, +d, 0, 1, -1, 0, 0));
	vertices.push_back(Mesh::VertexMesh(-w, +h, +d, 0, 0, -1, 0, 0));
	vertices.push_back(Mesh::VertexMesh(-w, +h, -d, 1, 0, -1, 0, 0));
	vertices.push_back(Mesh::VertexMesh(-w, -h, -d, 1, 1, -1, 0, 0));

	//Right
	vertices.push_back(Mesh::VertexMesh(+w, -h, -d, 0, 1, 1, 0, 0));
	vertices.push_back(Mesh::VertexMesh(+w, +h, -d, 0, 0, 1, 0, 0));
	vertices.push_back(Mesh::VertexMesh(+w, +h, +d, 1, 0, 1, 0, 0));
	vertices.push_back(Mesh::VertexMesh(+w, -h, +d, 1, 1, 1, 0, 0));
	
	indices = 
	{
		0, 1, 2, 0, 2, 3,
		4, 5, 6, 4, 6, 7,
		8, 9, 10, 8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 21, 22, 20, 22, 23
	};

	vertexBuffer = new VertexBuffer(&vertices[0], vertices.size(), sizeof(Mesh::VertexMesh));
	indexBuffer = new IndexBuffer(&indices[0], indices.size());
}


