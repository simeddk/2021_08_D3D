#include "stdafx.h"
#include "ObbDemo.h"

void ObbDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(10, -1, 0);
	Context::Get()->GetCamera()->Position(3, 2, -4);
	
	shader = new Shader(L"24_integration.fxo");
	
	transform[0] = new Transform();
	transform[0]->Position(2, 1, 1);
	transform[0]->Scale(2, 1, 1);
	collider[0] = new Collider(transform[0]);


	transform[1] = new Transform();
	transform[1]->Position(4, 1, 1);
	collider[1] = new Collider(transform[1]);
}

void ObbDemo::Destroy()
{
	SafeDelete(shader);

	SafeDelete(transform[0]);
	SafeDelete(transform[1]);

	SafeDelete(collider[0]);
	SafeDelete(collider[1]);
	
}

void ObbDemo::Update()
{
	Vector3 position;
	transform[0]->Position(&position);

	if (Keyboard::Get()->Press(VK_RIGHT))
		position.x += 3.0f * Time::Delta();
	else if (Keyboard::Get()->Press(VK_LEFT))
		position.x -= 3.0f * Time::Delta();

	if (Keyboard::Get()->Press(VK_UP))
		position.y += 3.0f * Time::Delta();
	else if (Keyboard::Get()->Press(VK_DOWN))
		position.y -= 3.0f * Time::Delta();

	transform[0]->Position(position);

	Vector3 position2;
	transform[1]->Position(&position2);

	if (Keyboard::Get()->Press(VK_NUMPAD6))
		position2.x += 3.0f * Time::Delta();
	else if (Keyboard::Get()->Press(VK_NUMPAD4))
		position2.x -= 3.0f * Time::Delta();

	if (Keyboard::Get()->Press(VK_NUMPAD8))
		position2.y += 3.0f * Time::Delta();
	else if (Keyboard::Get()->Press(VK_NUMPAD5))
		position2.y -= 3.0f * Time::Delta();

	transform[1]->Position(position2);


	Vector3 rotation = Vector3(0, 0, 0);
	Vector3 rotation2 = Vector3(0, 0, 0);

	transform[0]->RotationDegree(&rotation);
	transform[1]->RotationDegree(&rotation2);

	ImGui::SliderFloat3("Rotation", (float*)rotation, -179.9f, 179.9f);
	ImGui::SliderFloat3("Rotation2", (float*)rotation2, -179.9f, 179.9f);

	transform[0]->RotationDegree(rotation);
	transform[1]->RotationDegree(rotation2);

	collider[0]->Update();
	collider[1]->Update();
}

void ObbDemo::Render()
{
	bool b = collider[0]->Intersection(collider[1]);

	Color color = b ? Color(1, 1, 1, 1) : Color(0, 0, 0, 1);
	collider[0]->Render(color);
	collider[1]->Render(color);
}
