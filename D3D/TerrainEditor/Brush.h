#pragma once

class Brush
{
public:
	Brush(Shader* shader, Terrain* terrain);
	~Brush();

	void Update();
	void Render();

private:
	void RaiseHeight(float intensity);

private:
	struct BrushDesc
	{
		Color Color = D3DXCOLOR(0, 1, 0, 1);

		Vector3 Location;
		UINT Type = 0;

		UINT Range = 2;
		float Padding[3];
	} brushDesc;

private:
	struct LineDesc
	{
		Color Color = D3DXCOLOR(1, 1, 1, 1);

		UINT Visible = 1;
		float Thickness = 0.01f;
		float Size = 1.0f;
		float Padding;
	} lineDesc;

private:
	Shader* shader;
	Terrain* terrain;

	ConstantBuffer* brushBuffer;
	ID3DX11EffectConstantBuffer* sBrushBuffer;

	ConstantBuffer* lineBuffer;
	ID3DX11EffectConstantBuffer* sLineBuffer;
};