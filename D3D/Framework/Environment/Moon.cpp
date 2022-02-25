#include "Framework.h"
#include "Moon.h"

Moon::Moon(Shader * shader)
	: Renderer(shader)
{
	moon = new Texture(L"Environment/Moon.png");
	moonGlow = new Texture(L"Environment/MoonGlow.png");

	sMoon = shader->AsSRV("MoonMap");
	sAlpha = shader->AsScalar("MoonAlpha");


	VertexTexture vertices[6];

	vertices[0].Position = Vector3(-1.0f, -1.0f, 0.0f);
	vertices[1].Position = Vector3(-1.0f, +1.0f, 0.0f);
	vertices[2].Position = Vector3(+1.0f, -1.0f, 0.0f);
	vertices[3].Position = Vector3(+1.0f, -1.0f, 0.0f);
	vertices[4].Position = Vector3(-1.0f, +1.0f, 0.0f);
	vertices[5].Position = Vector3(+1.0f, +1.0f, 0.0f);

	vertices[0].Uv = Vector2(0, 1);
	vertices[1].Uv = Vector2(0, 0);
	vertices[2].Uv = Vector2(1, 1);
	vertices[3].Uv = Vector2(1, 1);
	vertices[4].Uv = Vector2(0, 0);
	vertices[5].Uv = Vector2(1, 0);

	vertexBuffer = new VertexBuffer(vertices, 6, sizeof(VertexTexture));
}

Moon::~Moon()
{
	SafeDelete(vertexBuffer);
}

void Moon::Update()
{
	Super::Update();
}

void Moon::Render(float theta)
{
	//UINT stride = sizeof(VertexTexture);
	//UINT offset = 0;

	vertexBuffer->Render();
	//D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	sAlpha->SetFloat(GetAlpha(theta));

	//Moon
	{
		Matrix W = GetTransform(theta);
		transform->World(W);

		Super::Render();

		sMoon->SetResource(moon->SRV());
		shader->Draw(0, Pass(), 6);
	}

	//Glow
	{
		Matrix W = GetGlowTransform(theta);
		transform->World(W);

		Super::Render();

		sMoon->SetResource(moonGlow->SRV());
		shader->Draw(0, Pass(), 6);
	}
}

float Moon::GetAlpha(float theta)
{
	if (theta < Math::PI * 0.5f || theta > Math::PI * 1.5f)
		return fabsf(sinf(theta + Math::PI * 0.5f));

	return 0.0f;
}

D3DXMATRIX Moon::GetTransform(float theta)
{
	Matrix S, R, T, D;

	//S
	D3DXMatrixScaling(&S, scale, scale, 1);

	//R
	///D3DXMatrixRotationYawPitchRoll(&R, Math::PI * 0.5f, theta - (Math::PI * 0.5f), 0);
	/*Matrix V = Context::Get()->View();
	D3DXMatrixInverse(&V, NULL, &V);
	Vector3 s, r, t;
	Math::MatrixDecompose(V, Vector3(), r, Vector3());
	D3DXMatrixRotationYawPitchRoll(&R, r.y, r.x, r.z);*/

	Vector3 r;
	Context::Get()->GetCamera()->Rotation(&r);
	D3DXMatrixRotationYawPitchRoll(&R, r.y, r.x, r.z);


	//T
	Vector3 position;
	Context::Get()->GetCamera()->Position(&position);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);

	//D
	Vector3 direction = Lighting::Get()->Direction();
	D3DXMatrixTranslation
	(
		&D
		, direction.x * distance
		, direction.y * distance
		, direction.z * distance
	);

	return S * R * T * D; //W matrix를 D만큼 이동
}

D3DXMATRIX Moon::GetGlowTransform(float theta)
{
	Matrix S, R, T, D;

	//S
	D3DXMatrixScaling(&S, glowScale, glowScale, 1);

	//R
	///D3DXMatrixRotationYawPitchRoll(&R, Math::PI * 0.5f, theta - (Math::PI * 0.5f), 0);
	/*Matrix V = Context::Get()->View();
	D3DXMatrixInverse(&V, NULL, &V);
	Vector3 s, r, t;
	Math::MatrixDecompose(V, s, r, t);
	D3DXMatrixRotationYawPitchRoll(&R, r.y, r.x, r.z);*/
	Vector3 r;
	Context::Get()->GetCamera()->Rotation(&r);
	D3DXMatrixRotationYawPitchRoll(&R, r.y, r.x, r.z); //항상 카메라를 바라보는 cpu 빌보드 효과

	//T
	Vector3 position;
	Context::Get()->GetCamera()->Position(&position);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);

	//D
	Vector3 direction = Lighting::Get()->Direction();
	D3DXMatrixTranslation
	(
		&D
		, direction.x * glowDistance
		, direction.y * glowDistance
		, direction.z * glowDistance
	);

	return S * R * T * D;
}