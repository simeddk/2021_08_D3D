#include "Framework.h"
#include "Freedom.h"

Freedom::Freedom()
{
}

Freedom::~Freedom()
{
}

void Freedom::Update()
{
	Vector3 forward = Forward();
	Vector3 up = Up();
	Vector3 right = Right();

	if (Mouse::Get()->Press(1) == false) return;

	//Move
	{
		Vector3 position;
		Position(&position);

		if (Keyboard::Get()->Press('W'))
			position += forward * moveSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('S'))
			position -= forward * moveSpeed * Time::Delta();

		if (Keyboard::Get()->Press('D'))
			position += right * moveSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('A'))
			position -= right * moveSpeed * Time::Delta();

		if (Keyboard::Get()->Press('Q'))
			position += up * moveSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('E'))
			position -= up * moveSpeed * Time::Delta();

		Position(position);
	}

	//Rotation
	{

	}
}

void Freedom::Speed(float moveSpeed, float rotationSpeed)
{
	this->moveSpeed = moveSpeed;
	this->rotationSpeed = rotationSpeed;
}
