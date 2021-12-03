#include "stdafx.h"
#include "VertexMultiLineDemo.h"

void VertexMultiLineDemo::Initialize()
{
	shader = new Shader(L"03_MultiLine.fx");

	vertices[0].Position = Vector3(0, 0, 0);
	vertices[1].Position = Vector3(1, 0, 0);

	vertices[2].Position = Vector3(0, 0.5, 0);
	vertices[3].Position = Vector3(1, 0.5, 0);

	vertices[4].Position = Vector3(0, -0.5, 0);
	vertices[5].Position = Vector3(1, -0.5, 0);

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = sizeof(Vertex) * 6;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = vertices;

	Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
}

void VertexMultiLineDemo::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
}

void VertexMultiLineDemo::Update()
{
}

void VertexMultiLineDemo::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	
	//프리미티브 토폴로지 변경 테스트
	D3D11_PRIMITIVE_TOPOLOGY topology[] = 
	{
		D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
		D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP
	};

	static bool bStirp;
	ImGui::Checkbox("Use Strip", &bStirp);

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(bStirp ? topology[1] : topology[0]);

	//패스 변경 테스트
	static UINT pass = 0;
	ImGui::InputInt("Pass", (int*)&pass);
	pass =  Math::Clamp<UINT>(pass, 0, 3);

	//벡터 파라미터 넘겨보기
	static Color color = Color(1, 1, 1, 1);
	ImGui::ColorEdit3("Color", (float*)&color);
	shader->AsVector("Color")->SetFloatVector(color);

	shader->Draw(0, pass, 6);
}


