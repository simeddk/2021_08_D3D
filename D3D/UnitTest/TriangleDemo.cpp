#include "stdafx.h"
#include "TriangleDemo.h"

void TriangleDemo::Initialize()
{
	shader = new Shader(L"04_MultiLine.fx");

	//Triagle - 0 
	{
		vertices[0].Position = Vector3(-0.5f, +0.0f, 0);
		vertices[1].Position = Vector3(+0.0f, +0.5f, 0);
		vertices[2].Position = Vector3(+0.5f, +0.0f, 0);

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(Vertex) * 3;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
	}

	//Triagle - 1 
	{
		vertices2[0].Position = Vector3(-0.5f, +0.0f - 0.5f, 0);
		vertices2[1].Position = Vector3(+0.0f, +0.5f - 0.5f, 0);
		vertices2[2].Position = Vector3(+0.5f, +0.0f - 0.5f, 0);

		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(Vertex) * 3;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices2;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer2));
	}
	
}

void TriangleDemo::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
	SafeRelease(vertexBuffer2);
}

void TriangleDemo::Update()
{
}

void TriangleDemo::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->Draw(0, 0, 3);

	static Color color = Color(1, 0, 0, 1);
	ImGui::ColorEdit3("Tri2 Color", color);
	shader->AsVector("Color")->SetFloatVector(color);

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer2, &stride, &offset);
	shader->Draw(0, 3, 3);
}


