#include "stdafx.h"
#include "Editor.h"


void Editor::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(20, 0, 0);
	Context::Get()->GetCamera()->Position(1, 36, -85);
	
	
}

void Editor::Destroy()
{
	
}

void Editor::Update()
{
	//램버트 테스트
	ImGui::SliderFloat3("LightDirection", Lighting::Get()->Direction(), -1, +1);
	
	
}

void Editor::Render()
{
	
}

