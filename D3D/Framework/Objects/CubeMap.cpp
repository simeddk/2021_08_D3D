#include "Framework.h"
#include "CubeMap.h"

CubeMap::CubeMap(Shader * shader)
	: shader(shader)
{
	mesh = new MeshSphere(shader, 0.5f);

	sSrv = shader->AsSRV("CubeMap");
}

CubeMap::~CubeMap()
{
	SafeDelete(mesh);
	SafeRelease(srv);
}

void CubeMap::Texture(wstring file)
{
	SafeRelease(srv);

	file = L"../../_Textures/" + file;
	Check(D3DX11CreateShaderResourceViewFromFile
	(
		D3D::GetDevice(), file.c_str(), nullptr, nullptr, &srv, nullptr
	));
}

void CubeMap::Update()
{
	mesh->Update();
}

void CubeMap::Render()
{
	sSrv->SetResource(srv);

	mesh->Render();
}

void CubeMap::Pass(UINT val)
{
	mesh->Pass(val);
}

void CubeMap::Position(float x, float y, float z)
{
	mesh->Position(x, y, z);
}

void CubeMap::Position(Vector3 & vec)
{
	mesh->Position(vec);
}

void CubeMap::Position(Vector3 * vec)
{
	mesh->Position(vec);
}

void CubeMap::Rotation(float x, float y, float z)
{
	mesh->Rotation(x, y, z);
}

void CubeMap::Rotation(Vector3 & vec)
{
	mesh->Rotation(vec);
}

void CubeMap::Rotation(Vector3 * vec)
{
	mesh->Rotation(vec);
}

void CubeMap::RotationDegree(float x, float y, float z)
{
	mesh->RotationDegree(x, y, z);
}

void CubeMap::RotationDegree(Vector3 & vec)
{
	mesh->RotationDegree(vec);
}

void CubeMap::RotationDegree(Vector3 * vec)
{
	mesh->RotationDegree(vec);
}

void CubeMap::Scale(float x, float y, float z)
{
	mesh->Scale(x, y, z);
}

void CubeMap::Scale(Vector3 & vec)
{
	mesh->Scale(vec);
}

void CubeMap::Scale(Vector3 * vec)
{
	mesh->Scale(vec);
}
