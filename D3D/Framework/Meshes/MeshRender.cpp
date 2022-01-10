#include "Framework.h"
#include "MeshRender.h"

MeshRender::MeshRender(Shader * shader, Mesh * mesh)
	: mesh(mesh)
{
	Pass(0);
	mesh->SetShader(shader);

	instanceWorldBuffer = new VertexBuffer(worlds, MAX_MESH_INSTANCE, sizeof(Matrix), 1, true);
	instanceColorBuffer = new VertexBuffer(colors, MAX_MESH_INSTANCE, sizeof(Color), 2, true);
}

MeshRender::~MeshRender()
{
	for (Transform* transform : transforms)
		SafeDelete(transform);

	SafeDelete(instanceWorldBuffer);
	SafeDelete(instanceColorBuffer);
	SafeDelete(mesh);
}

void MeshRender::Update()
{
	mesh->Update();
}

void MeshRender::Render()
{
	instanceWorldBuffer->Render();
	instanceColorBuffer->Render();

	mesh->Render(transforms.size());
}

Transform * MeshRender::AddTransform()
{
	Transform* transform = new Transform();
	transforms.push_back(transform);

	colors[transforms.size() - 1] = Color(0, 0, 0, 1);

	return transform;
}

void MeshRender::UpdateSubResource()
{
	for (UINT i = 0; i < transforms.size(); i++)
		memcpy(worlds[i], transforms[i]->World(), sizeof(Matrix));

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(instanceWorldBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, worlds, sizeof(Matrix) * MAX_MESH_INSTANCE);
	}
	D3D::GetDC()->Unmap(instanceWorldBuffer->Buffer(), 0);

	D3D::GetDC()->Map(instanceColorBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, colors, sizeof(Color) * MAX_MESH_INSTANCE);
	}
	D3D::GetDC()->Unmap(instanceColorBuffer->Buffer(), 0);
}

void MeshRender::SetColor(UINT instance, Color & color)
{
	colors[instance] = color;
}
