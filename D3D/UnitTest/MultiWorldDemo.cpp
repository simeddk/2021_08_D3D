#include "stdafx.h"
#include "MultiWorldDemo.h"

void MultiWorldDemo::Initialize()
{
	shader = new Shader(L"05_World.fx");

	//Create VertexData
	vertices[0].Position = Vector3(+0.0f, +0.0f, +0.0f);
	vertices[1].Position = Vector3(+0.0f, +0.5f, +0.0f);
	vertices[2].Position = Vector3(+0.5f, +0.0f, +0.0f);

	vertices[3].Position = Vector3(+0.5f, +0.0f, +0.0f);
	vertices[4].Position = Vector3(+0.0f, +0.5f, +0.0f);
	vertices[5].Position = Vector3(+0.5f, +0.5f, +0.0f);

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = sizeof(Vertex) * 6;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = vertices;

	Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));

	//Create World & Color
	for (int i = 0; i < 10; i++)
	{
		Vector3 scale = Math::RandomVec3(0.5f, 2.0f);
		Vector3 position = Math::RandomVec3(-3.0f, +3.0f);
		position.z = 0.0f;

		Matrix S, T;
		D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);
		D3DXMatrixTranslation(&T, position.x, position.y, position.z);
		
		worlds[i] = S * T;
		colors[i] = Math::RandomColor4();
	}
}

void MultiWorldDemo::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
}

void MultiWorldDemo::Update()
{
	//+, - 키를 통해 월드 인덱스 선택하기
	static UINT index = 0;
	if (Keyboard::Get()->Down(VK_ADD))
		index++;
	else if (Keyboard::Get()->Down(VK_SUBTRACT))
		index--;

	index = Math::Clamp<UINT>(index, 0, ARRAYSIZE(worlds) - 1);

	//텍스트 렌더
	ImVec4 textColor = ImVec4(colors[index].r, colors[index].g, colors[index].b, 1.0f);
	ImGui::TextColored(textColor, "[%d]", index);

	Vector2 position = Vector2(worlds[index]._41, worlds[index]._42);
	Vector2 scale = Vector2(worlds[index]._11, worlds[index]._22);
	ImGui::TextColored(textColor, "position %0.2f, %0.2f", position.x, position.y);
	ImGui::TextColored(textColor, "scale %0.2f, %0.2f", scale.x, scale.y);

	//스케일
	if (Keyboard::Get()->Press(VK_LSHIFT))
	{
		if (Keyboard::Get()->Press(VK_RIGHT))
			worlds[index]._11 += 2.0f * Time::Delta();
		else if (Keyboard::Get()->Press(VK_LEFT))
			worlds[index]._11 -= 2.0f * Time::Delta();

		if (Keyboard::Get()->Press(VK_UP))
			worlds[index]._22 += 2.0f * Time::Delta();
		else if (Keyboard::Get()->Press(VK_DOWN))
			worlds[index]._22 -= 2.0f * Time::Delta();
	}

	//이동
	else
	{
		if (Keyboard::Get()->Press(VK_RIGHT))
			worlds[index]._41 += 2.0f * Time::Delta();
		else if (Keyboard::Get()->Press(VK_LEFT))
			worlds[index]._41 -= 2.0f * Time::Delta();

		if (Keyboard::Get()->Press(VK_UP))
			worlds[index]._42 += 2.0f * Time::Delta();
		else if (Keyboard::Get()->Press(VK_DOWN))
			worlds[index]._42 -= 2.0f * Time::Delta();
	}
	
	//회전
	if (Keyboard::Get()->Press(VK_LCONTROL))
	{
		for (auto& W : worlds)
		{
			Vector3 scale, rotation, position;

			Math::MatrixDecompose(W, scale, rotation, position);
			rotation.z += 10.0f * Time::Delta();

			Matrix S, R, T;
			D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);
			D3DXMatrixTranslation(&T, position.x, position.y, position.z);
			D3DXMatrixRotationZ(&R, rotation.z);

			W = S * R * T;
		}
	}
	
	shader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	shader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());

}

void MultiWorldDemo::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	static UINT mode = 0;
	ImGui::SliderInt("WireFrame", (int*)&mode, 0, 1);

	for (UINT i = 0; i < 10; i++)
	{
		shader->AsVector("Color")->SetFloatVector(colors[i]);
		shader->AsMatrix("World")->SetMatrix(worlds[i]);
		shader->Draw(0, mode, 6);
	}


}


