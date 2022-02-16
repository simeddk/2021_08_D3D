#include "stdafx.h"
#include "Brush.h"

Brush::Brush(Shader * shader, Terrain * terrain)
	: shader(shader)
	, terrain(terrain)
{
	brushBuffer = new ConstantBuffer(&brushDesc, sizeof(BrushDesc));
	sBrushBuffer = shader->AsConstantBuffer("CB_TerrainBrush");
}

Brush::~Brush()
{
	SafeDelete(brushBuffer);
}

void Brush::Update()
{
	ImGui::Separator();

	ImGui::ColorEdit3("BrushColor", brushDesc.Color);
	ImGui::InputInt("BrushType", (int*)&brushDesc.Type);
	brushDesc.Type %= 3;

	ImGui::InputInt("BrushRange", (int*)&brushDesc.Range);
	brushDesc.Range = Math::Clamp<UINT>(brushDesc.Range, 1, 10);

	if (brushDesc.Type > 0)
	{
		brushDesc.Location = terrain->GetCursorPosition();

		if (brushDesc.Type == 1)
		{
			if (Mouse::Get()->Press(0))
				RaiseHeight(5.0f);
		}

	}
}

void Brush::Render()
{
	if (brushDesc.Type > 0)
	{
		string str = "";
		str += to_string(brushDesc.Location.x);
		str += ", ";
		str += to_string(brushDesc.Location.y);
		str += ", ";
		str += to_string(brushDesc.Location.y);

		Gui::Get()->RenderText(10, 50, 1, 0, 0, str);
	}

	brushBuffer->Render();
	sBrushBuffer->SetConstantBuffer(brushBuffer->Buffer());
}

void Brush::RaiseHeight(float intensity)
{
	UINT left = (UINT)brushDesc.Location.x - brushDesc.Range;
	UINT right = (UINT)brushDesc.Location.x + brushDesc.Range;
	UINT bottom = (UINT)brushDesc.Location.z - brushDesc.Range;
	UINT top = (UINT)brushDesc.Location.z + brushDesc.Range;

	if (left < 0) left = 0;
	if (right >= terrain->Width()) right = terrain->Width();
	if (bottom < 0) right = bottom = 0;
	if (top >= terrain->Height()) top = terrain->Height();

	for (UINT z = bottom; z <= top; z++)
	{
		for (UINT x = left; x <= right; x++)
		{
			UINT index = terrain->Width() * z + x;

			terrain->Vertices()[index].Position.y += intensity * Time::Delta();
		}
	}

	terrain->CreateNormalData();
	terrain->UpdateVertextData();
}
