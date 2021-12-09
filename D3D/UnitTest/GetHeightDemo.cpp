#include "stdafx.h"
#include "GetHeightDemo.h"

void GetHeightDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(6, 0, 0);
	Context::Get()->GetCamera()->Position(0, 26, -45);
	((Freedom*)Context::Get()->GetCamera())->Speed(50, 2);

	shader = new Shader(L"11_Terrain.fx");

	terrain = new Terrain(shader, L"Terrain/Gray256.png");
	terrain->BaseMap(L"Terrain/Cliff (Sandstone).jpg");

	triShader = new Shader(L"05_World.fx");
	vertices[0].Position = Vector3(0, 1, 0);
	vertices[1].Position = Vector3(-1, 0, 0);
	vertices[2].Position = Vector3(1, 0, 0);

	//Create VertexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(Vertex) * 3;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
	}
}

void GetHeightDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(terrain);

	SafeDelete(triShader);
	SafeRelease(vertexBuffer);
}

void GetHeightDemo::Update()
{
	//조명 방향 테스트
	static Vector3 LightDirection = Vector3(-1, -1, +1);
	ImGui::SliderFloat3("Light Direction", (float*)LightDirection, -1, 1);
	shader->AsVector("LightDirection")->SetFloatVector(LightDirection);

	//파이프라인 변경 테스트
	static UINT pass = shader->PassCount() - 1;
	ImGui::InputInt("Pass", (int*)&pass);
	pass %= shader->PassCount();
	terrain->Pass() = pass;

	//알베도 or 램버트 모드 테스트
	if (pass == 5)
	{
		static UINT albedo = 1;
		ImGui::RadioButton("Albedo", (int*)&albedo, 1);
		ImGui::RadioButton("Lambert", (int*)&albedo, 2);
		shader->AsScalar("Albedo")->SetInt(albedo);
	}

	terrain->Update();

	//역삼각형 키보드 이동 & GetHeight 테스트
	if (Keyboard::Get()->Press(VK_RIGHT))
		position.x += 20.0f * Time::Delta();
	else if (Keyboard::Get()->Press(VK_LEFT))
		position.x -= 20.0f * Time::Delta();

	if (Keyboard::Get()->Press(VK_UP))
		position.z += 20.0f * Time::Delta();
	else if (Keyboard::Get()->Press(VK_DOWN))
		position.z -= 20.0f * Time::Delta();

	position.y = terrain->GetHeight_Raycast(position) + 1.0f;

	Matrix R, T;
	D3DXMatrixRotationX(&R, Math::PI);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);
	
	Matrix world = R * T;

	triShader->AsMatrix("World")->SetMatrix(world);
	triShader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	triShader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());

	//역삼각형 위치 텍스트렌더로 찍어보기
	string str = to_string(position.x) + ", " + to_string(position.y) + ", " + to_string(position.z);
	Gui::Get()->RenderText(5, 60, str);
}

void GetHeightDemo::Render()
{
	terrain->Render();

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	triShader->Draw(0, 2, 3);


}


