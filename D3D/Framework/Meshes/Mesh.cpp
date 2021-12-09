#include "Framework.h"
#include "Mesh.h"

Mesh::Mesh(Shader * shader)
	: shader(shader)
{
	D3DXMatrixIdentity(&world);

	sWorld = this->shader->AsMatrix("World");
	sView = this->shader->AsMatrix("View");
	sProjection = this->shader->AsMatrix("Projection");

	sDiffuseMap = this->shader->AsSRV("DiffuseMap");
}

Mesh::~Mesh()
{
	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);

	SafeRelease(vertexBuffer);
	SafeRelease(indexBuffer);

	SafeDelete(diffuseMap);
}

void Mesh::Update()
{
}

void Mesh::Render()
{
	if (vertexBuffer == nullptr || indexBuffer == nullptr)
	{
		Create();
		CreateBuffer();
	}


	UINT stride = sizeof(VertexMesh);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	sWorld->SetMatrix(world);
	sView->SetMatrix(Context::Get()->View());
	sProjection->SetMatrix(Context::Get()->Projection());

	if (diffuseMap != nullptr)
		sDiffuseMap->SetResource(diffuseMap->SRV());

	shader->DrawIndexed(0, pass, indexCount);
	
}

void Mesh::Position(float x, float y, float z)
{
}

void Mesh::Position(Vector3 & vec)
{
}

void Mesh::Position(Vector3 * vec)
{
}

void Mesh::Rotation(float x, float y, float z)
{
}

void Mesh::Rotation(Vector3 & vec)
{
}

void Mesh::Rotation(Vector3 * vec)
{
}

void Mesh::RotationDegree(float x, float y, float z)
{
}

void Mesh::RotationDegree(Vector3 & vec)
{
}

void Mesh::RotationDegree(Vector3 * vec)
{
}

void Mesh::Scale(float x, float y, float z)
{
}

void Mesh::Scale(Vector3 & vec)
{
}

void Mesh::Scale(Vector3 * vec)
{
}

Vector3 Mesh::Forward()
{
	return Vector3();
}

Vector3 Mesh::Up()
{
	return Vector3();
}

Vector3 Mesh::Right()
{
	return Vector3();
}

void Mesh::DiffuseMap(wstring file)
{
}

void Mesh::CreateBuffer()
{
	//Create VertexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(VertexMesh) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
	}

	//Create IndexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(UINT) * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = indices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &indexBuffer));
	}
}

void Mesh::UpdateWorld()
{
}
