#pragma once
#include "Camera.h"

class Fixity : public Camera
{
public:
	Fixity();

	void Update() override;
};