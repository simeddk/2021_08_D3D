#include "Framework.h"
#include "CubeSky.h"

CubeSky::CubeSky(wstring file)
{
	shader = new Shader(L"14_CubeSky.fx");
	sphere = new MeshRender(shader, new MeshSphere(0.5f));
	sphere->AddTransform();

	file = L"../../_Textures/" + file;
	Check(D3DX11CreateShaderResourceViewFromFile
	(
		D3D::GetDevice(), file.c_str(), nullptr, nullptr, &srv, nullptr
	));

	sSrv = shader->AsSRV("SkyCubeMap");

}

CubeSky::~CubeSky()
{
	SafeDelete(shader);
	SafeDelete(sphere);
	SafeRelease(srv);
}

void CubeSky::Update()
{
	Vector3 cameraPosition;
	Context::Get()->GetCamera()->Position(&cameraPosition);
	sphere->GetTransform(0)->Position(cameraPosition);

	sphere->Update();
}

void CubeSky::Render()
{
	sSrv->SetResource(srv);
	sphere->Render();
}
